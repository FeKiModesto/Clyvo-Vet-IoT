import cv2

# CARREGA A FOTO DO PET
img = cv2.imread("pet.jpg")

if img is None:
    print("ERRO: Nao achei a foto pet.jpg na pasta!")
else:
    print("--- CLYVO VET: ANALISANDO IMAGEM DO PACIENTE ---")
    
    # PEGA O TAMANHO DA FOTO PARA DESENHAR O QUADRADO
    altura, largura, _ = img.shape
    
    # DESENHA UM QUADRADO VERDE NA FOTO (SIMULANDO A IA ACHANDO O PET)
    # O quadrado vai ficar no meio da imagem
    cv2.rectangle(img, (int(largura*0.2), int(altura*0.2)), 
                  (int(largura*0.8), int(altura*0.8)), (0, 255, 0), 5)
    
    cv2.putText(img, "PET DETECTADO", (int(largura*0.2), int(altura*0.15)), 
                cv2.FONT_HERSHEY_SIMPLEX, 1.2, (0, 255, 0), 3)

    print("Status: Animal identificado. Triagem IoT liberada!")
    
    # MOSTRA A FOTO PROCESSADA NA TELA
    cv2.imshow("Visao Computacional - Clyvo Vet", img)
    print("\nPressione qualquer tecla na janela da imagem para fechar...")
    cv2.waitKey(0) 
    cv2.destroyAllWindows()

