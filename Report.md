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

