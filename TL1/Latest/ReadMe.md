# 実装内容
* オブジェクトの出力
  * トランスフォームを出力しBlender上と同じ様に配置できます
  * また、カメラの情報を元に配置できるのでタイトルなど動かないカメラの配置に役立てます
  * コンポーネント志向により設計したクラスにより読み込んだ内容によって共通のクラスのインスタンスで様々な機能分岐ができます
  * 今のところはカメラとただの箱、それらにコリジョンボックスを追加したもののみ配置できます