#ifndef HANDLE_FILE_H
    #define HANDLE_FILE_H
        #include "escola.h"

        // funcionalidade 1
        void file_read_csv_write_binary(const char *nome_arq_dados, const char *nome_arq_binario); // funcao para ler do csv e escrever no arquivo em binario
        // funcionalidade 2
        void file_read_all_binary(const char *nome_arq_binario); // funcao para recuperar todos os registros do arquivo binario
        // funcionalidade 4
        void file_read_binary_rrn(const char *nome_arq_binario, const int rrn); // funcao para buscar um registro dado seu RRN
        // funcionalidade 5
        void file_delete_record(const char *nome_arq_binario, int rrn); // funcao para remover um registro dado seu RRN
        // funcionalidade 9
        void file_print_stack(const char *nome_arq_binario); // funcao para imprimir a pilha
#endif
