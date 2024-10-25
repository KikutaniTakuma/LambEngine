# 自分が書いてないコード
- Projects
  - Game
    - Src
      - Editor
        - BlockEditor
        - EditorManager
        - GoalManager
        - WhirlpoolEditor
        - WindWditor
      - Game
        - Water以外
      - Scenes
        - GameScene
        - SelectStageScene
---
* ゲームの部分のコードはチームメンバーが書いています
* 各シーンの基底クラスや大元は自分で書きました
---
<br> <br> 

# エンジン
## エンジンで最もこだわっているところ
- Projects
  - LambEngine
    - Engine
      - Graphics
        - [RenderingManager](https://github.com/KikutaniTakuma/LambEngine/tree/master/Projects/LambEngine/Engine/Graphics/RenderingManager)
        - RenderContextManager
          - [RenderContext](https://github.com/KikutaniTakuma/LambEngine/tree/master/Projects/LambEngine/Engine/Graphics/RenderContextManager/RenderContext)
      - [Drawer](https://github.com/KikutaniTakuma/LambEngine/tree/master/Projects/LambEngine/Drawer)
---
* 描画は[RenderingManager](https://github.com/KikutaniTakuma/LambEngine/tree/master/Projects/LambEngine/Engine/Graphics/RenderingManager)で管理してます。
  * RenderContextの情報をもとに実際に描画コマンドを発行します
  * レンダーターゲットの設定、ポストエフェクト等もここで行います
* 描画情報の格納は[RenderContext](https://github.com/KikutaniTakuma/LambEngine/tree/master/Projects/LambEngine/Engine/Graphics/RenderContextManager/RenderContext)
* 描画情報を積んでいるのは[Drawer](https://github.com/KikutaniTakuma/LambEngine/tree/master/Projects/LambEngine/Drawer)
---
## コンポーネント志向
- Projects
  - Game
    - Src
      - [GameObject](https://github.com/KikutaniTakuma/LambEngine/tree/master/Projects/Game/Src/GameObject)
        - [Manager](https://github.com/KikutaniTakuma/LambEngine/tree/master/Projects/Game/Src/GameObject/Manager)
---
* コンポーネント志向に基づくオブジェクトの設計
* エディターでオブジェクトの追加と削除、コンポーネントの追加
* エディターでTransformの編集
* 実際のゲーム内で使用したのは2024年の10daysのみです。今後水中のオブジェクトの配置をする際にはこれを使用する予定です
---
