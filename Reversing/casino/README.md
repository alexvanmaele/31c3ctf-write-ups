# casino

**Category:** Reversing
**Points:** 20
**Description:**

> Let's play a game!
>
> ```bash
> nc 188.40.18.77 2000
> ```

## Write-up


Casino was a game about betting money and guessing numbers. [The source](casino.ey) was provided, but we did not recognize the language.

This is what a normal game looked like:


```bash
$ nc 188.40.18.77 2000
Welcome to the casino. Your commands are: 'account', 'flag', and 'I bet <1234> and guess <1234>'
account
You have 10 units
flag
No flag for you
play the game!
I bet 5 and guess 20
you were wrong the number was 46
I bet 5 and guess 46
you were wrong the number was 123
You lost!
```

The source [indicated](casino.ey#L28) that we had to get one million units in order to win the game and show the flag.

After playing the game several times, we noticed it would generate numbers between 0 and 200. Since we only had 10 units to begin with, we decided it would take us too long to win the game legitimately.

Instead, we began fuzzing the game parameters and observing how the game reacted to uncommon values:

```bash
account
You have 10 units
I bet 0 and guess 10  
you were wrong the number was 128
account
You have 10 units
I bet -5 and guess 10
I dont understand
I bet 999 and guess 10
you cant bet that much!
account
You have 10 units
I bet AAA and guess 10
I dont understand
I bet 10 and guess AAA
I dont understand
I bet 2E and guess 1C
I dont understand
account
You have 10 units
```

The game validated the input correctly so we proceeded to try different things. e.g. What about long values?

```bash
account
You have 10 units
I bet 99999999999999999999999999999999 and guess 10
you were wrong the number was 129
account
You have 8814407033341083659 units
flag
OK, you won: 31C3_033fda2193ec453ed838609c6fdb5aec
```

Oops :-)

The flag is `31C3_033fda2193ec453ed838609c6fdb5aec`.