# dtoa

This `dtoa` is provided as a single C file to demonstrate an "easy" algorithm to handle 64-bits IEEE 754 double.

## Principles

A basic floating-point number work under the same principle as an unsigned integer: it is the sum of power of 2. For an `unsigned int`, the range of exponent is `[0;32]`. For a `double`, this range is `[i-53;i]` with `i` being in the range `[-1022;1023]`.

Here is a pseudo-code to describe what is happening:

```
unit = 1
if (i - 53 >= 0)
  for (j = 0; j < i-53; j++)
    unit *= 2
else
  for(j = 0; j > i-53; j--)
    unit /= 2
result = 0
for (j = 0; j<mantissa_size;j++)
   if ((mantissa >> j) & 1)
      result += unit
   unit *= 2
```

In our code we build some sort of bigint that can handle multiplication by 2, division by 2, and addition. We sacrifice memory efficiency by storing the result in base 10 (by segment of 9 digits). This make printing the final result easier. This `dtoa` will return a malloc'd string with full precision.

## Demonstration:

```
~/Documents/dtoa > gcc dtoa.c
~/Documents/dtoa > ./a.out 123.123
Printf conversion: 123.1230000000000046611603465862572193145751953125000000000000000000000000000000000000000000000000000000
Dtoa conversion:   123.1230000000000046611603465862572193145751953125
~/Documents/dtoa > ./a.out 42.42
Printf conversion: 42.4200000000000017053025658242404460906982421875000000000000000000000000000000000000000000000000000000
Dtoa conversion:   42.4200000000000017053025658242404460906982421875
~/Documents/dtoa > ./a.out 1e23
Printf conversion: 99999999999999991611392.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
Dtoa conversion:   99999999999999991611392
~/Documents/dtoa > ./a.out 0
Printf conversion: 0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
Dtoa conversion:   0
~/Documents/dtoa > ./a.out -0.123456789
Printf conversion: -0.1234567889999999973360544913703051861375570297241210937500000000000000000000000000000000000000000000
Dtoa conversion:   -0.12345678899999999733605449137030518613755702972412109375
~/Documents/dtoa >
```

## Optimization

This code is not "optimized" and I don't intend to "optimize" it. It is good enough for my use case.

## Correctness

Does not handle denormalized value (because I am lazy). I cannot guarantee the solidity of the `string`. I did not run extensive test.
