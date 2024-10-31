# LambEngine
## 水の描画に特化したエンジン
* DeferredRendering
* ZSrot
* 水面のシミュレーション
* ポストエフェクト
* MeshShaderとVertexShaderの切り替え

## 水概要
* 制作期間 : 11ヶ月
* PerlinNoiseからHeightMapを作成、NormalMapに変換して細かい波の表現しています。
* 水を透過させて水面下に484個のモデルを配置。DoubleBufferとMeshShaderで描画を最適化しています。
* 水を描画する際に歪みの情報をPerlinNoiseで生成。それを元にポストエフェクトで歪ませています。
* 深度値を使用して水面下かつ近くにあるものだけコースティクスを加算して描画しています。

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
  * ZSortをRenderingManagerのDraw関数でしています
* 描画情報の格納は[RenderContext](https://github.com/KikutaniTakuma/LambEngine/tree/master/Projects/LambEngine/Engine/Graphics/RenderContextManager/RenderContext)
* 描画情報を積んでいるのは[Drawer](https://github.com/KikutaniTakuma/LambEngine/tree/master/Projects/LambEngine/Drawer)
* 水面のシミュレーションは[PixelShader](https://github.com/KikutaniTakuma/LambEngine/blob/master/Projects/Game/Shaders/WaterTex2DShader/WaterTex2D.PS.hlsl)
で情報を書き込みをし、[DeferredRendering](https://github.com/KikutaniTakuma/LambEngine/tree/master/Projects/Game/Shaders/DeferredRendering)でライティングしている
---

<br> 

# アプリケーション
## ゲーム : 笹舟大航海
* ジャンル : シミュレーション
* 作成期間 : 2ヶ月
* 制作人数 : 3人(プランナー1人、プログラマー2人)
* 担当箇所 : 水、空、タイトル、描画エンジン
## チームメンバーが書いたコード
- Projects
  - Game
    - Src
      - [GameExternals](https://github.com/KikutaniTakuma/LambEngine/tree/master/Projects/Game/Src/GameExternals)
---
* ゲームの部分のコードはチームメンバーが書いています
* 各シーンの基底クラスや大元は自分で書きました
---