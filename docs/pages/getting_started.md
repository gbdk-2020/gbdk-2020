@page docs_getting_started Getting Started


Follow the steps in this section to start using GBDK-2020.


# 1. Compile Example projects
Make sure your GBDK-2020 installation is working correctly by compiling some of the included @ref docs_example_programs "example projects".

Navigate to the example projects folder (`"examples/gb/"` under your GBDK-2020 install folder) and open a command line. Then type:

    make

This should build all of the examples sequentially. You can also navigate into an individual example project's folder and build it by typing `make`.

If everything works and there are no errors reported each example sub-folder should have it's on .gb ROM file.


# 2. Use a Template
__To create a new project use a template!__ 

There are template projects included in the @ref docs_example_programs "GBDK example projects" to help you get up and running. Their folder names start with `template_`.

1. Copy one of the template folders to a new folder name

2. If you moved the folder out of the GBDK examples then you __must__ update the `GBDK` path variable and/or the path to `LCC` in the `Makefile` or `make.bat` so that it will still build correctly.

3. Type `make` on the command line in that folder to verify it still builds.

4. Open main.c to start making changes.


# 3. If you use GBTD / GBMB, get the fixed version
If you plan to use GBTD / GBMB for making graphics, make sure to get the version with the `const` fix and other improvements. See @ref const_gbtd_gbmb.


# 4. Review Coding Guidelines
Take a look at the @ref docs_coding_guidelines "coding guidelines", even if you have experience writing software for other platforms. There is important information to help you get good results and performance on the Game Boy.

If you haven't written programs in C before, check the @ref docs_c_tutorials "C tutorials section".


# 5. Try a GBDK Tutorial
You might want to start off with a guided GBDK tutorial from the @ref links_gbdk_tutorials "GBDK Tutorials section".

  - __Note:__ Tutorials (or parts of them) may be based on the older GBDK from the 2000's before it was updated to be GBDK-2020. The general principals are all the same, but the setup and parts of the @ref docs_toolchain "toolchain" (compiler/etc) may be somewhat different and some links may be outdated (pointing to the old GBDK or old tools).


# 6. Read up!
- It is strongly encouraged to read more @ref docs_index "GBDK-2020 General Documentation".
- Learn about the Game Boy hardware by reading through the @ref Pandocs technical reference.


# 7. Need help?
Check out the links for @ref links_help_and_community "online community and support".