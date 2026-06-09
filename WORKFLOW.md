# **Commands available to run in OSS-Fuzz**
Remember to run the commands from the `/oss-fuzz` directory.

## **Run a fuzzer**
### 1. Build image
```
python3 infra/helper.py build_image $PROJECT_NAME
```

### 2. Build fuzzers
```
python3 infra/helper.py build_fuzzers --sanitizer=address $PROJECT_NAME
```
#### Build fuzzers and clean old artifacts
```
python3 infra/helper.py build_fuzzers $PROJECT_NAME --clean
```

#### Use the following directory to work with the corpus
```
/home/ivan/Code/oss-fuzz/build/corpus/$PROJECT_NAME/$HARNESS
```

### 3. Run libFuzzer with local parameters and seeds
#### Important! Remember to save the new seeds in the local directory
```
python3 infra/helper.py run_fuzzer $PROJECT_NAME $HARNESS -- -fork=N -max_len=1024 -timeout=2 -rss_limit_mb=2048 -max_total_time=3600 -reduce_inputs=1 -shrink=1 -entropic=1 -use_value_profile=1 -print_pcs=1 -reload=30
```

## **Obtain coverage data**
### 1. Compile in coverage mode
```
python3 infra/helper.py build_fuzzers --sanitizer=coverage $PROJECT_NAME
```

### 2. Pass the corpus
```
python3 infra/helper.py coverage --fuzz-target=<fuzz_target_name> \
    --corpus-dir=<my_local_corpus_dir> $PROJECT_NAME
```

### 3. Coverage of the entire project
```
python3 infra/helper.py coverage --no-corpus-download $PROJECT_NAME
```

## **Obtain introspection data**
### 1. First, get coverage
```
python3 infra/helper.py coverage --no-corpus-download $PROJECT_NAME
```
#### Use public corpora from oss-fuzz
```
python3 infra/helper.py introspector $PROJECT_NAME --public-corpora
```
### 2. Generate an introspection report with the local corpus (if it exists)
```
python3 infra/helper.py introspector $PROJECT_NAME
```
#### Get only a coverage report
```
python3 infra/helper.py introspector $PROJECT_NAME --coverage-only
```
### 3. Check the report
```
python3 -m http.server 8008 --directory /home/my_user/oss-fuzz/build/out/$PROJECT_NAME/introspector-report/inspector
```
Navigate to `localhost:8008/fuzz_report.html` in your browser.

## **Reproduce a testcase**
```
python3 infra/helper.py reproduce $PROJECT_NAME <fuzz_target_name> <testcase_path>
```

## **Run the container manually**
```
docker run -it \
  --cap-add=SYS_PTRACE \
  --security-opt seccomp=unconfined \
  -v $(pwd)/corpus/$PROJECT_NAME/$HARNESS:/corpus \
  -e CORPUS_DIR=/corpus \
  gcr.io/oss-fuzz/$PROJECT_NAME \
  /out/fuzzer_runner $HARNESS
```
