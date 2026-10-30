# ChupChup

FPS competitivo em Unreal Engine 5 (C++), com mecânica inspirada em CS e Valorant. A arte ainda é provisória (formas básicas da engine); toda a jogabilidade já está em código.

## O que já tem

- **Movimento competitivo:** aceleração e frenagem altas (dá para fazer counter-strafe), Shift para andar devagar, Ctrl/C para agachar com câmera suave, pulo.
- **Armas hitscan com spray pattern fixo**, que dá para aprender como no CS:
  - **Rifle** (estilo AK): automático, 600 RPM, 30 balas, headshot de um tiro só.
  - **Pistola**: semiautomática, precisa no primeiro tiro e penaliza quem atira rápido demais.
- **Precisão realista:** o primeiro tiro é preciso parado; erra muito correndo (acima de 34% da velocidade) e no ar; melhora agachado.
- **Recuo:** parte do recuo move a câmera e o resto só desvia as balas; a mira volta ao centro depois do spray.
- **Hitbox de cabeça** com multiplicador de dano.
- **Animação procedural da arma:** coice, recarga, troca de arma e balanço ao andar.
- **HUD:** mira dinâmica (abre conforme a imprecisão real), hit marker (amarelo = headshot, vermelho = kill), dano, vida, munição e estatísticas (kills, precisão, % de headshot).
- **Mapa de treino gerado por código:** área de spawn, parede central com duas portas, caixas, rampa até uma plataforma elevada e 8 bots (alguns se movendo de um lado para o outro) que renascem 1,5 s depois de morrer.

## Controles

| Ação | Teclado/mouse | Controle |
| --- | --- | --- |
| Mover | WASD | Analógico esquerdo |
| Olhar | Mouse | Analógico direito |
| Atirar | Botão esquerdo | RT |
| Recarregar | R | X |
| Agachar | Ctrl ou C | B |
| Andar devagar | Shift | — |
| Pular | Espaço | A |
| Rifle / Pistola / Última arma | 1 / 2 / Q | Y (última arma) |

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

## Jogando

Com o editor aberto, clique em **Play** (Alt+P). O mapa padrão é o template básico da engine (`/Engine/Maps/Templates/Template_Default`), que já tem luz e céu. O `ChupChupGameMode` monta a arena de treino automaticamente e coloca você no spawn.

- Se o template não abrir na sua versão, crie uma fase com `File → New Level → Basic` e aperte Play: a arena é criada em qualquer mapa.
- Para usar um mapa seu sem a arena, desmarque `bSpawnTrainingArena` num Blueprint filho do `ChupChupGameMode`.
- A sensibilidade do mouse fica em `MouseSensitivity` no personagem. Os valores das armas (dano, RPM, spray pattern, spread) ficam nos construtores de `ChupChupRifle.cpp` e `ChupChupPistol.cpp`.

## Colocando arte de verdade

A jogabilidade não depende da arte provisória. Para chegar no visual de um jogo pronto:

- **Armas e braços:** pegue os assets do template *First Person* ou do **Lyra Starter Game** (grátis na Epic) e troque as peças (`BodyPart`, `BarrelPart`...) de `AChupChupWeapon` por um Skeletal Mesh, ou crie um Blueprint filho do rifle e mude as malhas.
- **Mapas:** monte um nível com assets do **Fab / Quixel Megascans** e desligue a arena gerada.
- **Som:** preencha `FireSound` nas armas (Blueprint filho) com um `.wav` importado.

## Estrutura

```
ChupChup.uproject
Config/                      Configurações (Engine, Game, Input)
Source/
  ChupChup.Target.cs         Alvo do jogo
  ChupChupEditor.Target.cs   Alvo do editor
  ChupChup/                  Módulo principal
    ChupChupGameMode.*         Regras, HUD e criação da arena
    ChupChupCharacter.*        Jogador FPS (movimento, inventário, estatísticas)
    ChupChupWeapon.*           Arma base (hitscan, spray, spread, recarga, animação)
    ChupChupRifle.* / ChupChupPistol.*
    ChupChupHealthComponent.*  Vida e eventos de dano/morte
    ChupChupTargetBot.*        Bot de treino com hitbox de cabeça
    ChupChupTrainingArena.*    Mapa de treino procedural
    ChupChupHUD.*              Mira dinâmica, hit marker e informações
    ChupChupPlayerController.*
```

As teclas estão definidas em `Config/DefaultInput.ini` (mapeamentos legados de eixo/ação).
