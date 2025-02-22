Reciprocal{#dev_guide_op_reciprocal}
====================================

## General

Reciprocal operation is element-wise Power operation where exponent(power) equals to -1. Reciprocal of 0 is infinity.

\f[dst = \begin{cases} src^{-1} & \text{if}\ src \neq 0  \\
    inf & \text{if}\ src = 0 \end{cases} \f]

## Operation attributes

Reciprocal operation does not support any attribute.

## Execution arguments

The inputs and outputs must be provided according to below index order when
constructing an operation.

### Inputs

| Index | Argument Name | Required or Optional |
|:------|:--------------|:---------------------|
| 0     | `src`         | Required             |

### Outputs

| Index | Argument Name | Required or Optional |
|:------|:--------------|:---------------------|
| 0     | `dst`         | Required             |

## Supported data types

Reciprocal operation supports the following data type combinations.

| Source | Destination |
|:-------|:------------|
| f32    | f32         |
| bf16   | bf16        |
| f16    | f16         |
