# DSAA6000D Final Project

**Han Linghu**

## Introduction

HKUST(GZ) DSAA6000D course project. 

Implementation and analysis of 2-hop labeling based on TOL. 

Baseline Ferrari: https://github.com/steps/Ferrari

## Compile

*Makefile* is provided. 

Use command *make* to generate object files and executable file *2hop*.

Use command *make clean* to delete all the intermediate files. 

## Sample Execution

Due to GitHub storage limitation, only DAG3k and its random query set is provided. For more datasets and query set, please contact the author.

### How to run the sample

After compiling, run the following command under 2hop directory,

```bash
./2hop -g ./graphs/DAG3kDense.in -q ./query/DAG3kDense_random.in 
```







