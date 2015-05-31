# sprobujmy jak w dict-editor najpierw plik wykonywalny dla dict-check
add_executable (dict-check dict-check.c)

# i zalinkowac biblioteke
target_link_libraries (dict-check dictionary)

