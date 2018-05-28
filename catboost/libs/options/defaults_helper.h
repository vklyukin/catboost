#pragma once

#include "enums.h"
#include "cat_feature_options.h"
#include "catboost_options.h"

#include <util/system/types.h>
#include <util/generic/ymath.h>

const ui32 FoldPermutationBlockSizeNotSet = 0;

inline int DefaultFoldPermutationBlockSize(int docCount) {
    return Min(256, docCount / 1000 + 1);
}

inline void UpdateCtrTargetBordersOption(ELossFunction lossFunction, ui32 approxDim, NCatboostOptions::TCtrDescription* ctr) {
    if (NeedTargetClassifier(ctr->Type)) {
        if (IsMultiClassError(lossFunction)) {
            ctr->TargetBinarization->BorderCount = approxDim - 1;
        }
    }
}

inline void UpdateCtrsTargetBordersOption(ELossFunction lossFunction, ui32 approxDim, TVector<NCatboostOptions::TCtrDescription>* ctrs) {
    for (auto& ctr : *ctrs) {
        UpdateCtrTargetBordersOption(lossFunction, approxDim, &ctr);
    }
}

inline void UpdateCtrsTargetBordersOption(ELossFunction lossFunction, ui32 approxDim, NCatboostOptions::TCatFeatureParams* catFeatureParams) {
    UpdateCtrsTargetBordersOption(lossFunction, approxDim, &catFeatureParams->SimpleCtrs.Get());
    UpdateCtrsTargetBordersOption(lossFunction, approxDim, &catFeatureParams->CombinationCtrs.Get());
    for (auto& perFeatureCtr : catFeatureParams->PerFeatureCtrs.Get()) {
        UpdateCtrsTargetBordersOption(lossFunction, approxDim, &perFeatureCtr.second);
    }
}

inline void UpdateBoostingTypeOption(size_t learnSampleCount, NCatboostOptions::TOption<EBoostingType>* boostingTypeOption) {
    if (boostingTypeOption->NotSet()) {
        if (learnSampleCount >= 50000) {
            *boostingTypeOption = EBoostingType::Plain;
        } else {
            *boostingTypeOption = EBoostingType::Ordered;
        }
    }
}

inline void UpdateUseBestModel(bool hasTest, bool hasTestConstTarget, NCatboostOptions::TOption<bool>* useBestModel) {
    if (useBestModel->NotSet() && hasTest && !hasTestConstTarget) {
        *useBestModel = true;
    }
    if (!hasTest && *useBestModel) {
        MATRIXNET_WARNING_LOG << "You should provide test set for use best model. use_best_model parameter swiched to false value." << Endl;
        *useBestModel = false;
    }
}

inline void UpdateLeavesEstimation(bool hasWeights, NCatboostOptions::TCatBoostOptions* catBoostOptions) {
    auto& leavesEstimationMethod = catBoostOptions->ObliviousTreeOptions->LeavesEstimationMethod;
    auto& leavesEstimationIterations = catBoostOptions->ObliviousTreeOptions->LeavesEstimationIterations;
    if (hasWeights && IsBinaryClassError(catBoostOptions->LossFunctionDescription->GetLossFunction())) {
        if (leavesEstimationMethod.NotSet()) {
            leavesEstimationMethod = ELeavesEstimation::Gradient;
        }
        if (leavesEstimationIterations.NotSet()) {
            leavesEstimationIterations = 40;
        }
    }
}

inline void UpdateLearningRate(int learnObjectCount, NCatboostOptions::TCatBoostOptions* catBoostOptions) {
    auto& learningRate = catBoostOptions->BoostingOptions->LearningRate;
    const bool doUpdateLearningRate = (
        learningRate.NotSet() &&
        catBoostOptions->BoostingOptions->IterationCount.NotSet() &&
        IsBinaryClassError(catBoostOptions->LossFunctionDescription->GetLossFunction()) &&
        catBoostOptions->ObliviousTreeOptions->LeavesEstimationMethod.NotSet() &&
        catBoostOptions->ObliviousTreeOptions->LeavesEstimationIterations.NotSet() &&
        catBoostOptions->ObliviousTreeOptions->L2Reg.NotSet()
    );
    if (doUpdateLearningRate) {
        const double a = 0.28318903;
        const double b = -6.04369765;
        // TODO(nikitxskv): Don't forget to change formula when add l2-leaf-reg depending on weights.
        const double logLearningRate = a * log(learnObjectCount) + b;
        learningRate = exp(logLearningRate);
        MATRIXNET_WARNING_LOG << "Learning rate set to " << learningRate << "." << Endl;
    }
}

inline void SetDataDependantDefaults(
    int learnPoolSize,
    int testPoolSize,
    bool hasTestConstTarget,
    bool hasWeights,
    NCatboostOptions::TOption<bool>* useBestModel,
    NCatboostOptions::TCatBoostOptions* catBoostOptions
) {
    UpdateUseBestModel(testPoolSize, hasTestConstTarget, useBestModel);
    UpdateBoostingTypeOption(learnPoolSize, &catBoostOptions->BoostingOptions->BoostingType);
    UpdateLearningRate(learnPoolSize, catBoostOptions);

    // TODO(nikitxskv): Remove it when the l2 normalization will be added.
    UpdateLeavesEstimation(hasWeights, catBoostOptions);
}
