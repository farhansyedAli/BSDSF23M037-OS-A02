Feature 1:

Task 1: Repository Creation

Created a public GitHub repository named BSDSF23M037-OS-A02, initialized it with a README file, and cloned it to the local machine.

Task 2: Adding Starter Code

Added the provided ls-v1.0.0.c file to the src folder and included a Makefile for compilation.

Task 3: Project Structure

Created the required directories: bin, obj, man, and src, along with the REPORT.md file to complete the project structure.

Task 4: Build and Testing

Successfully compiled the program using the Makefile and verified its functionality by running the executable, which listed files correctly.

Task 5: Commit and Push

Committed all files and pushed the complete structured project to the GitHub repository.


Feature 2:

Q1. What is the crucial difference between the stat() and lstat() system calls? In the context of the ls command, when is it more appropriate to use lstat()?
Answer:
The stat() system call retrieves information about the file that a pathname points to. If the path is a symbolic link, stat() follows the link and returns information about the target file.
The lstat() system call, however, returns information about the link itself, not the file it references.
In the context of the ls command, lstat() is more appropriate because ls -l must show details of the symbolic link itself (its permissions, owner, and link target), rather than the file the link points to.

Q2. The st_mode field in struct stat is an integer that contains both the file type (e.g., regular file, directory) and the permission bits. Explain how you can use bitwise operators (like &) and predefined macros (like S_IFDIR or S_IRUSR) to extract this information.
Answer:
The st_mode field stores file type and permission bits together using a combination of binary flags.
You can use bitwise AND (&) with predefined macros to extract specific information:

To check the file type, use:

if (st.st_mode & S_IFDIR)  // checks if it's a directory


To check owner permissions, use:

if (st.st_mode & S_IRUSR)  // owner has read permission
if (st.st_mode & S_IWUSR)  // owner has write permission
if (st.st_mode & S_IXUSR)  // owner has execute permission


These macros help you interpret the mode bits, allowing you to print file types (like d for directories) and permission strings (rwxr-xr-x) in the ls -l format.

Feature 3:

Q1. Explain the general logic for printing items in a "down then across" columnar format. Why is a simple single loop through the list of filenames insufficient for this task?

In the "down then across" format, the program first stores all filenames in an array and determines how many rows and columns fit on the screen based on the terminal width and the longest filename.
Printing happens row by row — for each row, we print the items from each column that belong in that row (e.g., filenames[0], filenames[0 + num_rows], filenames[0 + 2*num_rows], etc.).

A simple single loop is insufficient because it would only print filenames in one continuous list (top to bottom), without correctly organizing them into aligned columns that adjust dynamically to the screen size.

Q2. What is the purpose of the ioctl system call in this context? What would be the limitations of your program if you only used a fixed-width fallback (e.g., 80 columns) instead of detecting the terminal size?

The ioctl system call retrieves the current terminal’s width in columns (TIOCGWINSZ). This allows the program to calculate how many filenames can fit per row and dynamically adjust the number of columns based on the user’s terminal size.

If only a fixed-width fallback (like 80 columns) were used, the output would not adapt to different terminal sizes — it might appear cramped on small screens or waste space on wider terminals, breaking alignment and usability.

Feature 4:

1. Difference in Implementation Complexity

The “down then across” (vertical) display logic is more complex than the “across” (horizontal) format.
It requires pre-calculating both rows and columns, determining how many filenames can fit per column, and printing in a specific order using index jumps (e.g., filenames[r + c × rows]).
In contrast, the horizontal method only tracks the current line width and wraps when the next filename would exceed the terminal width, so it needs less pre-computation.

2. Managing Multiple Display Modes (-l, -x, default)

A flag variable (e.g., display_mode) was used to store the selected mode based on the parsed command-line options:

-l → calls do_ls_long() for long listing format

-x → calls do_ls_horizontal() for horizontal layout

default (no option) → calls do_ls() for vertical “down then across” layout

This simple branching structure ensures only the correct display function runs for each mode.


Feature 5:

Q1. Why is it necessary to read all directory entries into memory before you can sort them? What are the potential drawbacks of this approach for directories containing millions of files?

Answer:
Sorting requires access to all elements at once so that they can be compared and rearranged. Therefore, all directory entries must first be read into memory before calling a sorting function like qsort().

The drawback is that this approach uses memory proportional to the number of files — if a directory contains millions of files, it can lead to high memory consumption, possible slow performance, or even memory allocation failures on systems with limited RAM.

Q2. Explain the purpose and signature of the comparison function required by qsort(). How does it work, and why must it take const void * arguments?

Answer:
qsort() is a generic sorting function that works on any data type. To achieve this flexibility, it relies on a user-defined comparison function that tells it how to compare two elements.

The function signature is:

int compare(const void *a, const void *b);


Inside the function, the pointers are cast to the correct type (e.g., const char ** for strings), and then compared — for example, using strcmp() for alphabetical order.

It takes const void * arguments because qsort() doesn’t know the type of data being sorted — this allows it to be type-agnostic and reusable for any data structure.



Feature 6:

Q1. How do ANSI escape codes work to produce color in a standard Linux terminal? Show the specific code sequence for printing text in green.
Answer:
ANSI escape codes are special character sequences that instruct the terminal to change the style or color of the text. Each sequence begins with \033[ (the escape character and bracket), followed by color/style codes, and ends with m.
For example, to print text in green, the sequence is:

printf("\033[0;32mThis text is green\033[0m\n");


Here,

0 → resets all attributes,

32 → sets the foreground color to green,

\033[0m → resets color back to default after printing.

Q2. To color an executable file, you need to check its permission bits. Explain which bits in the st_mode field you need to check to determine if a file is executable by the owner, group, or others.
Answer:
In the st_mode field of struct stat, the executable permissions are represented by specific bit flags:

S_IXUSR → executable by the owner

S_IXGRP → executable by the group

S_IXOTH → executable by others

To check if a file is executable, you use bitwise AND operations, for example:

if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
    // file is executable


If any of these bits are set, the file should be displayed in the color designated for executables (commonly green).


