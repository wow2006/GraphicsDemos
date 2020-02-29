#!/bin/env bash

for i in {1..4}
do
  perf stat ./mimicOpenGL $i 2> >(grep misses)
done

