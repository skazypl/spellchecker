# dodajemy bibliotekę dictionary, stworzoną na podstawie pliku dictionary.c
# biblioteka będzie dołączana statycznie (czyli przez linkowanie pliku .o)

add_library (dictionary dictionary.c word_list.c trie.c set.c)

if (CMOCKA)
    # dodajemy plik wykonywalny z testem    
    add_executable (word_list_test word_list.c word_list_test.c)

    # i linkujemy go z biblioteką do testowania
    target_link_libraries (word_list_test ${CMOCKA})

    # wreszcie deklarujemy, że to test
    add_test (word_list_unit_test word_list_test)
    add_test (word_list_val valgrind 
    
        --error-exitcode=1 --read-var-info=yes
         --leak-check=full --show-leak-kinds=all ./word_list_test)
    

    add_executable (set_test set.c set_test.c)
    target_link_libraries (set_test ${CMOCKA})
    add_test (set_unit_test set_test)
    add_test (set_val valgrind 
    
        --error-exitcode=1 --read-var-info=yes
         --leak-check=full --show-leak-kinds=all ./set_test)
    

    add_executable (trie_test trie.c set.c trie_test.c)
    target_link_libraries (trie_test ${CMOCKA})
    add_test (trie_unit_test trie_test)
    
    add_test (trie_val valgrind 
    
        --error-exitcode=1 --read-var-info=yes
         --leak-check=full --show-leak-kinds=all ./trie_test)


    add_executable (trie_impl_test set.c trie_impl_test.c)
    target_link_libraries (trie_impl_test ${CMOCKA})
    add_test (trie_impl_unit_test trie_impl_test)    
    add_test (trie_impl_val valgrind 
    
        --error-exitcode=1 --read-var-info=yes
         --leak-check=full --show-leak-kinds=all ./trie_impl_test)    
    

    add_executable (trie_io_test trie.c set.c trie_io_test.c)
    set_target_properties (trie_io_test
        PROPERTIES 
        COMPILE_DEFINITIONS 
        UNIT_TESTING=1)
    target_link_libraries (trie_io_test ${CMOCKA})
    add_test (trie_io_unit_test trie_io_test)
    add_test (trie_io_val valgrind 
    
        --error-exitcode=1 --read-var-info=yes
         --leak-check=full --show-leak-kinds=all ./trie_io_test)
    

    add_executable (dictionary_test dictionary.c trie.c set.c word_list.c dictionary_test.c)
    target_link_libraries (dictionary_test ${CMOCKA})
    add_test (dictionary_unit_test dictionary_test)    
    add_test (dictionary_val valgrind 
    
        --error-exitcode=1 --read-var-info=yes
         --leak-check=full --show-leak-kinds=all ./dictionary_test)    
    
    add_executable (dictionary_impl_test trie.c set.c word_list.c dictionary_impl_test.c)
    target_link_libraries (dictionary_impl_test ${CMOCKA})
    add_test (dictionary_impl_unit_test dictionary_impl_test)    
    add_test (dictionary_impl_val valgrind 
    
        --error-exitcode=1 --read-var-info=yes
         --leak-check=full --show-leak-kinds=all ./dictionary_impl_test)    
    


endif (CMOCKA)
