# pCRAPp

**Category:** Web

**Points:** 10

**Description:**

> PHP is nasty crappy sometimes, just pwn it
>
> http://188.40.18.69/

## Write-up

The server contained two files: [flag.php](flag.php) and [pCRAPp.php](pCRAPp.php). *Note: flag.php is our own version, but it should be identical.*

The pCRAPp.php file printed its own source, describing how to solve this challenge. The goal was to put [all four variables](pCRAPp.php#L34) to TRUE, in order to print the flag. This challenge focused on some weird behaviour in PHPs JSON decoding and string comparing functions.

We initially passed the test locally by using following request:

`pCRAPp.php?a={"a1":"4000%202","a2":[[1],0,3,4,5]}&b=1.15.15&c[]=531c3&c[][]=1&d=%00`

However, this did not work remotely. After doing a recon on the server, we found the only difference in our setup was the use of Windows (locally) vs Ubuntu (remotely). Sure enough, there was a PHP bug specific to Ubuntu that allowed our request to pass remotely as well:

`pCRAPp.php?a={"a1":"4000%202","a2":[[1],0,3,4,5]}&b=001&c[]=531c3&c[][]=1&d=%00`

Note the b parameter: `b=001` instead of `b=1.15`.

This finally printed the flag: `31c3_pHp_h4z_f41l3d_d34l_w1tH_1T`