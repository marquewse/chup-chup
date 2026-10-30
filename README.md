# ChupChup

Projeto Unreal Engine 5 (C++) com um esqueleto genérico: personagem em terceira pessoa com movimento básico (WASD/analógico, mouse/analógico direito para a câmera, Espaço/A para pular).

## Requisitos (Windows)

- Unreal Engine 5.5 (instalado pelo Epic Games Launcher). Se usar outra versão 5.x, ajuste `EngineAssociation` em `ChupChup.uproject`.
- Visual Studio 2022 com as cargas de trabalho **Desenvolvimento para desktop com C++** e **Desenvolvimento de jogos com C++** (inclui o Windows SDK).
- [Git LFS](https://git-lfs.com/) instalado (`git lfs install`), pois `.uasset`, `.umap` e mídia são rastreados por LFS.

## Como abrir e compilar

1. Clone o repositório:
   ```bash
   git clone https://github.com/marquewse/chup-chup.git
   ```
2. No Explorador de Arquivos, clique com o botão direito em `ChupChup.uproject` → **Generate Visual Studio project files**. Isso cria `ChupChup.sln`.
3. Abra `ChupChup.sln`, selecione a configuração **Development Editor** e a plataforma **Win64**, e compile o alvo `ChupChup` (Ctrl+Shift+B).
4. Dê um duplo clique em `ChupChup.uproject` para abrir o editor (ou rode pelo Visual Studio com F5).

## Primeiros passos no editor

- Abra ou crie uma fase (`File → New Level`) com um chão e um **Player Start**.
- O `ChupChupGameMode` já é o GameMode padrão (Project Settings → Maps & Modes) e usa `AChupChupCharacter` e `AChupChupPlayerController`.
- Salve a fase em `Content/` e defina-a como mapa padrão, se desejar.

## Estrutura

```
ChupChup.uproject
Config/                      Configurações (Engine, Game, Input)
Source/
  ChupChup.Target.cs         Alvo do jogo
  ChupChupEditor.Target.cs   Alvo do editor
  ChupChup/                  Módulo principal
    ChupChupGameMode.*
    ChupChupCharacter.*
    ChupChupPlayerController.*
```

As teclas estão definidas em `Config/DefaultInput.ini` (mapeamentos legados de eixo/ação).
