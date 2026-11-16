#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define ID_PRODUTO 101
#define ID_QTD     102
#define ID_PRECO   103
#define ID_SALVAR  104
#define ID_MOSTRAR 105
#define ID_LISTA   106
#define ARQUIVO "aval1234.txt"

HWND campoProduto, campoQtd, campoPreco, campoLista;

/* Converte texto em numero e valida: 0 = invalido, 1 = ok */
int lerNumero(const char *s, double *saida) {
    char *fim;
    errno = 0;
    double numero = strtod(s, &fim);
    if (s == fim || errno == ERANGE || numero <= 0 || numero > 1000000)
        return 0;
    *saida = numero;
    return 1;
}

void salvarCompra() {
    char produto[100], txtQtd[30], txtPreco[30];
    double qtd, preco;

    GetWindowTextA(campoProduto, produto, sizeof(produto));
    GetWindowTextA(campoQtd, txtQtd, sizeof(txtQtd));
    GetWindowTextA(campoPreco, txtPreco, sizeof(txtPreco));

    for (char *p = txtPreco; *p; p++) if (*p == ',') *p = '.';

    if (produto[0] == '\0' || !lerNumero(txtQtd, &qtd) || !lerNumero(txtPreco, &preco)) {
        MessageBoxA(NULL, "Dados invalidos! Preencha o produto e use valores positivos.",
                    "Erro", MB_OK | MB_ICONERROR);
        return;
    }

    int quantidade = (int)qtd;
    double total = quantidade * preco;

    FILE *arquivo = fopen(ARQUIVO, "a");
    if (!arquivo) {
        MessageBoxA(NULL, "Nao foi possivel abrir o arquivo.", "Erro", MB_OK | MB_ICONERROR);
        return;
    }
    fprintf(arquivo, "%s;%d;%.2f;%.2f\n", produto, quantidade, preco, total);
    fclose(arquivo);

    SetWindowTextA(campoProduto, "");
    SetWindowTextA(campoQtd, "");
    SetWindowTextA(campoPreco, "");
    MessageBoxA(NULL, "Compra salva com sucesso!", "Aviso", MB_OK | MB_ICONINFORMATION);
}

void mostrarCompras() {
    char linha[256], produto[100], texto[300];
    int quantidade, contador = 0;
    double preco, total, acumulado = 0;

    SendMessageA(campoLista, LB_RESETCONTENT, 0, 0);

    FILE *arquivo = fopen(ARQUIVO, "r");
    if (!arquivo) {
        SendMessageA(campoLista, LB_ADDSTRING, 0, (LPARAM)"Nenhuma compra cadastrada.");
        return;
    }

    while (fgets(linha, sizeof(linha), arquivo)) {
        if (sscanf(linha, "%99[^;];%d;%lf;%lf", produto, &quantidade, &preco, &total) == 4) {
            sprintf(texto, "%s | Qtd: %d | Preco: R$ %.2f | Total: R$ %.2f",
                    produto, quantidade, preco, total);
            SendMessageA(campoLista, LB_ADDSTRING, 0, (LPARAM)texto);
            acumulado += total;
            contador++;
        }
    }
    fclose(arquivo);

    sprintf(texto, "--- %d compra(s) | Valor acumulado: R$ %.2f ---", contador, acumulado);
    SendMessageA(campoLista, LB_ADDSTRING, 0, (LPARAM)texto);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        CreateWindowA("STATIC", "Produto:",    WS_VISIBLE | WS_CHILD, 20, 22, 100, 20, hwnd, NULL, NULL, NULL);
        CreateWindowA("STATIC", "Quantidade:", WS_VISIBLE | WS_CHILD, 20, 57, 100, 20, hwnd, NULL, NULL, NULL);
        CreateWindowA("STATIC", "Preco (R$):", WS_VISIBLE | WS_CHILD, 20, 92, 100, 20, hwnd, NULL, NULL, NULL);

        campoProduto = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                     130, 20, 300, 25, hwnd, (HMENU)ID_PRODUTO, NULL, NULL);
        campoQtd     = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                     130, 55, 300, 25, hwnd, (HMENU)ID_QTD, NULL, NULL);
        campoPreco   = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                     130, 90, 300, 25, hwnd, (HMENU)ID_PRECO, NULL, NULL);

        CreateWindowA("BUTTON", "Salvar compra", WS_VISIBLE | WS_CHILD,
                      130, 130, 140, 30, hwnd, (HMENU)ID_SALVAR, NULL, NULL);
        CreateWindowA("BUTTON", "Mostrar compras", WS_VISIBLE | WS_CHILD,
                      290, 130, 140, 30, hwnd, (HMENU)ID_MOSTRAR, NULL, NULL);

        campoLista = CreateWindowA("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL,
                                   20, 180, 540, 200, hwnd, (HMENU)ID_LISTA, NULL, NULL);
        break;
    case WM_COMMAND:
        if (LOWORD(wp) == ID_SALVAR)  salvarCompra();
        if (LOWORD(wp) == ID_MOSTRAR) mostrarCompras();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcA(hwnd, msg, wp, lp);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int show) {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "JanelaCompras";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA("JanelaCompras", "Cadastro de Compras",
                              WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
                              200, 100, 600, 450, NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, show);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 0;
}
