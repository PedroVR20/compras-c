# Cadastro de Compras (C + Win32)

Programa em C com interface gráfica (windows.h) para cadastrar compras.

- Campos: produto, quantidade e preço (com validação)
- Botão **Salvar compra**: calcula o total e grava em `aval1234.txt` (`produto;qtd;preco;total`)
- Botão **Mostrar compras**: lê o arquivo, lista os registros e mostra o valor acumulado

## Compilar e executar (MinGW)

```
gcc compras.c -o compras.exe -mwindows
./compras.exe
```
