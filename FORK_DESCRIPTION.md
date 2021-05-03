# What this fork do?
This fork addresses the following issue: #1524. This issue is a request for adding the feature for returning fitted models after cross-validation. It is an effective way to get an ensemble of models, which were trained on different subsets of the source data.  

# How to use?
When using the `cv` method, set `return_models=True`. Fitted models will be returned as a list of `CatBoost` objects. The list will be added as a second variable.

## Documentation:
```
...
    return_models: bool, optional (default=False)
       if True, return a list of models fitted for each CV fold

    Returns
    -------
    cv results : pandas.core.frame.DataFrame with cross-validation results
        columns are: test-error-mean  test-error-std  train-error-mean  train-error-std
    cv models : list of trained models, if return_models=True
...
```

# Author
Valeriy Klyukin
