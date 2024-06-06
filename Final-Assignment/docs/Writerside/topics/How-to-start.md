# How to start

This topic provides information how to setup and run application. 

> **Before you start**
>
> Download flex, bison, make. Add them to PATH.
>
{style="note"}

## How to build program

Run make command

   ```bash
    make all
   ```
## Program Usage

### Run program in Interactive Mode
To run program in Interactive mode follow instruction:

1. Run program in your terminal

   ```bash
   ./prog
   ```
   
2. Enter your query 

   ![InterativeMode](InterativeMode.png)
3. Finish your query with **EOF** signal.


> You can simulate EOF with CTRL+D (for *nix) or CTRL+Z then Enter (for Windows) from command line.

4. You will get output in files:
 - `out.S` -- assembly file
 - `tree` -- tree file in a readable form

### Run program in File Read Mode

1. Run program with file name as argument in your terminal:

```bash
   ./prog tests/2.t
   ```
2. You will get output in files:
- `out.S` -- assembly file
- `tree` -- tree file in a readable form