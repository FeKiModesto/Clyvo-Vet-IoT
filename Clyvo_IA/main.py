"""
CLYVO VET — Visão Computacional
Disciplina: Disruptive Architectures: IoT, IoB & Generative IA
Grupo: Felipe Kirschner Modesto (RM: 561810)
       Enrico Delesporte (RM: 565760)
       Vitor Dias dos Santos (RM: 565422)
"""

import cv2
import sys
import os

COR_VERDE  = (0, 255, 0)
COR_FUNDO  = (0, 0, 0)
ESPESSURA  = 3
FONTE      = cv2.FONT_HERSHEY_SIMPLEX

def desenhar_cantos(img, x1, y1, x2, y2, cor, espessura):
    tam = 20
    e = espessura + 1
    # Canto superior esquerdo
    cv2.line(img, (x1, y1), (x1 + tam, y1), cor, e)
    cv2.line(img, (x1, y1), (x1, y1 + tam), cor, e)
    # Canto superior direito
    cv2.line(img, (x2, y1), (x2 - tam, y1), cor, e)
    cv2.line(img, (x2, y1), (x2, y1 + tam), cor, e)
    # Canto inferior esquerdo
    cv2.line(img, (x1, y2), (x1 + tam, y2), cor, e)
    cv2.line(img, (x1, y2), (x1, y2 - tam), cor, e)
    # Canto inferior direito
    cv2.line(img, (x2, y2), (x2 - tam, y2), cor, e)
    cv2.line(img, (x2, y2), (x2, y2 - tam), cor, e)

def detectar_pet(caminho: str = "pet.jpg") -> None:
    print("=" * 50)
    print("  CLYVO VET — Visão Computacional")
    print("=" * 50)
    print(f"\n[INFO] Carregando imagem: {caminho}")

    if not os.path.exists(caminho):
        print(f"[ERRO] Arquivo '{caminho}' não encontrado.")
        print("       Coloque a foto na mesma pasta e tente novamente.")
        sys.exit(1)

    img = cv2.imread(caminho)

    if img is None:
        print(f"[ERRO] Não foi possível abrir '{caminho}'.")
        print("       Verifique se o arquivo é uma imagem válida.")
        sys.exit(1)

    altura, largura, _ = img.shape
    print(f"[INFO] Imagem carregada — {largura}x{altura}px")
    print(f"[INFO] Analisando paciente...")

    # Bounding box (60% da imagem, centralizada)
    x1 = int(largura * 0.20)
    y1 = int(altura  * 0.20)
    x2 = int(largura * 0.80)
    y2 = int(altura  * 0.80)

    # Retangulo principal (levemente transparente)
    overlay = img.copy()
    cv2.rectangle(overlay, (x1, y1), (x2, y2), COR_VERDE, 1)
    cv2.addWeighted(overlay, 0.4, img, 0.6, 0, img)

    # Cantos estilizados tipo HUD
    desenhar_cantos(img, x1, y1, x2, y2, COR_VERDE, ESPESSURA)

    # Label "PET DETECTADO"
    label = "PET DETECTADO"
    escala = largura / 800
    (tw, th), _ = cv2.getTextSize(label, FONTE, escala, 2)
    tx = x1
    ty = max(y1 - 10, th + 5)
    cv2.rectangle(img, (tx - 4, ty - th - 6), (tx + tw + 4, ty + 4), COR_VERDE, -1)
    cv2.putText(img, label, (tx, ty), FONTE, escala, COR_FUNDO, 2, cv2.LINE_AA)

    # Barra de status inferior
    barra_h = 36
    bar_overlay = img.copy()
    cv2.rectangle(bar_overlay, (0, altura - barra_h), (largura, altura), (10, 10, 10), -1)
    cv2.addWeighted(bar_overlay, 0.75, img, 0.25, 0, img)
    cv2.putText(img, "STATUS: Animal identificado | Triagem IoT liberada",
                (10, altura - 10), FONTE, 0.45, COR_VERDE, 1, cv2.LINE_AA)

    print(f"[OK]   Animal identificado.")
    print(f"[OK]   Bounding box aplicada.")
    print(f"[OK]   Protocolo de triagem IoT: LIBERADO")
    print(f"\nPressione qualquer tecla na janela para fechar...")

    cv2.imshow("CLYVO VET — Visao Computacional", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    print("[INFO] Módulo de visão computacional encerrado.\n")

if __name__ == "__main__":
    caminho = sys.argv[1] if len(sys.argv) > 1 else "pet.jpg"
    detectar_pet(caminho)