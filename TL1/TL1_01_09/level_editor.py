import bpy
import math
import bpy_extras
import gpu
import gpu_extras.batch
import copy

#ブレンダーに登録するアドオン情報
bl_info = {
    "name" : "レベルエディタ",
    "author" : "Taro Kamata",
    "version" : (1, 0),
    "blender" : (3,3,1),
    "location" : "",
    "description" : "レベルエディタ",
    "warning" : "",
    #"support" : "TESTING",
    "wiki_url" : "",
    "tracker_url" : "",
    "category" : "Object"
}

#オペレータ シーン出力
class MYADDON_OT_export_scene(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
    bl_idname = "myaddon.myaddon_ot_export_scene"
    bl_label = "シーン出力"
    bl_description = "シーン情報をExportします"
    #出力するファイルの拡張子
    filename_ext = ".scene"

    def write_and_print(self, file, str):
        print(str)

        file.write(str)
        file.write('\n')

    def parse_sceme_recursive(self, file, object, level):
        """シーン解析用再帰関数"""

        indent = ' '
        for i in range(level):
            indent += "\t"

        #オブジェクト名書き込み
        self.write_and_print(file, indent + object.type + " - " + object.name)
                
        #ローカルトランスフォーム行列から平行移動、回転、スケーリングを抽出
        #型は Vector, Quternion, Vector
        trans, rot, scale = object.matrix_local.decompose()
        #回転をQuternion から Euler(3軸での回転角) に変換
        rot = rot.to_euler()
        #ラジアンから度数法に変換
        rot.x = math.degrees(rot.x)
        rot.y = math.degrees(rot.y)
        rot.z = math.degrees(rot.z)

        #トランスフォーム情報を表示
        self.write_and_print(file, indent + "T(%f,%f,%f)" % (trans.x, trans.y, trans.z))
        self.write_and_print(file, indent + "R(%f,%f,%f)" % (rot.x, rot.y, rot.z))
        self.write_and_print(file, indent + "S(%f,%f,%f)" % (scale.x, scale.y, scale.z))
        #カスタムプロパティ'file_name'
        if "file_name" in object:
            self.write_and_print(file, indent + "N %s" % object["file_name"])
        self.write_and_print(file, indent + 'END')
        self.write_and_print(file, '')


        for child in object.children:
            self.parse_sceme_recursive(file, child, level + 1)
        

    def export(self):
        """ファイルに出力"""

        print("シーン情報出力開始... %r" % self.filepath)

        #ファイルをテキスト形式で書きだしようにオープン
        #スコープを抜けると自動的にクローズされる
        with open(self.filepath, "wt") as file:

            #ファイルに文字列を書き込む
            file.write("SCENE\n")
            #シーン全てのオブジェクト
            for object in bpy.context.scene.objects:
                if(object.parent):
                    continue

                self.parse_sceme_recursive(file, object, 0)



    def execute(self, context):
        print("シーン情報をExportします")

        #ファイルに出力
        self.export()

        
        print("シーン情報をExportしました")
        self.report({'INFO'}, "シーン情報をExportしました")

        return {'FINISHED'}
    


#オペレータ 頂点を伸ばす
class MYADDON_OT_streatch_vertex(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_streatch_vertex"
    bl_label = "頂点を伸ばす"
    bl_description = "頂点座標を引っ張って伸ばします"
    #リドゥ、アンドゥ可能オプション
    bl_options = {'REGISTER', 'UNDO'}

    #メニューを実行したときに呼ばれるコールバック関数
    def execute(self, context):
        bpy.data.objects["Cube"].data.vertices[0].co.x += 1.0
        print("頂点を伸ばしました")

        #オペレータの命令終了を通知
        return {'FINISHED'}
    

#オペレータ ICO球生成
class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_ico_sphere"
    bl_label = "ICO球生成"
    bl_description = "ICO球を生成します"
    #リドゥ、アンドゥ可能オプション
    bl_options = {'REGISTER', 'UNDO'}

    #メニューを実行したときに呼ばれるコールバック関数
    def execute(self, context):
        bpy.ops.mesh.primitive_ico_sphere_add()
        print("ICO球を生成しました")

        #オペレータの命令終了を通知
        return {'FINISHED'}

class TOPBAR_MT_my_menu(bpy.types.Menu):
    bl_idname = "TOPBAR_MT_my_menu"
    bl_label = "MyMenu"
    bl_description = "拡張メニュー by " + bl_info["author"]

    #サブメニューの描画
    def draw(self, context):
        self.layout.operator(MYADDON_OT_export_scene.bl_idname,
            text=MYADDON_OT_export_scene.bl_label)

        self.layout.separator()
        #トップバーの「エディタメニュー」に項目(オペレータ)を追加
        self.layout.operator(MYADDON_OT_streatch_vertex.bl_idname,
            text=MYADDON_OT_streatch_vertex.bl_label)
        #トップバーの「エディタメニュー」に項目(オペレータ)を追加
        self.layout.operator(MYADDON_OT_create_ico_sphere.bl_idname,
            text=MYADDON_OT_create_ico_sphere.bl_label)
        
        self.layout.separator()

        self.layout.operator("wm.url_open_preset", 
            text="Manual", icon = 'HELP')
        
    #既存のメニューにサブメニューを追加
    def submenu(self, context):

        #ID指定でサブメニュー追加
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)



class MYADDON_OT_add_filename(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_filename"
    bl_label = "Filename 追加"
    bl_description = "['file_name']カスタムプロパティを追加します"
    bl_options = {"REGISTER", "UNDO"}

    def execute(self, context):
        #['file_name']カスタムプロパティを追加
        context.object["file_name"] = ""

        return {'FINISHED'}
    

#パネル ファイル名
class OBJECT_PT_file_name(bpy.types.Panel):
    """オブジェクトのファイルネームパネル"""
    bl_idname = "OBJECT_PT_file_name"
    bl_label = "FileName"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):
        #パネルに項目を追加
        if "file_name" in context.object:
            #すでにプロパティがあれば、プロパティを表示
            self.layout.prop(context.object, '["file_name"]', text = self.bl_label)
        else:
            #プロパティがなければ、プロパティ追加ボタンを表示
            self.layout.operator(MYADDON_OT_add_filename.bl_idname)

#コライダー描画
class DrawCollider:

    #描画ハンドル
    handle = None
    #3Dビューに登録する描画関数
    def darw_collider():
        #頂点データ
        vertices = {"pos":[]}
        #インデックスデータ
        indices = []

        #各頂点の、オブジェクト中心からのオフセット
        offsets = [
            [-0.5,-0.5,-0.5],
            [+0.5,-0.5,-0.5],
            [-0.5,+0.5,-0.5],
            [+0.5,+0.5,-0.5],
            [-0.5,-0.5,+0.5],
            [+0.5,-0.5,+0.5],
            [-0.5,+0.5,+0.5],
            [+0.5,+0.5,+0.5],
        ]

        #立方体のサイズ
        size = [2,2,2]

        #現在のオブジェクトリストを走査
        for object in bpy.context.scene.objects:
            start = len(vertices["pos"])

            for offset in offsets:
                pos = copy.copy(object.location)
                pos[0] += offset[0]*size[0]
                pos[1] += offset[1]*size[1]
                pos[2] += offset[2]*size[2]
                vertices['pos'].append(pos)
                
                indices.append([start+0, start+1])
                indices.append([start+2, start+3])
                indices.append([start+0, start+2])
                indices.append([start+1, start+3])

                indices.append([start+4, start+5])
                indices.append([start+6, start+7])
                indices.append([start+4, start+6])
                indices.append([start+5, start+7])

                indices.append([start+0, start+4])
                indices.append([start+1, start+5])
                indices.append([start+2, start+6])
                indices.append([start+3, start+7])


        #ビルドインのシェーダを取得
        shader = gpu.shader.from_builtin("3D_UNIFORM_COLOR")

        #バッチを作成
        batch = gpu_extras.batch.batch_for_shader(shader, "LINES", vertices, indices = indices)

        #シェーダのパラメータ
        color = [0.5, 1.0, 1.0, 1.0]
        shader.bind()
        shader.uniform_float("color", color)
        #描画
        batch.draw(shader)

#Blenderに登録するクラスリスト
classes = (
    MYADDON_OT_export_scene,
    MYADDON_OT_create_ico_sphere,
    MYADDON_OT_streatch_vertex,
    TOPBAR_MT_my_menu,
    MYADDON_OT_add_filename,
    OBJECT_PT_file_name,
    #DrawCollider,
)

# メニュー項目画面
def draw_menu_manual(self, context):
    self.layout.operator("wm.url_open_preset", text="Manual", icon = 'HELP')

#アドオン有効化時のこーるバック
def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)
    #3Dビューみ描画関数を追加
    DrawCollider.handle = bpy.types.SpaceView3D.draw_handler_add(DrawCollider.darw_collider, (), "WINDOW", "POST_VIEW")
    print("レベルエディタが有効化されました。")

#アドオン無効化時コールバック
def unregister():
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)
    bpy.types.SpaceView3D.draw_handler_remove(DrawCollider.handle, "WINDOW")

    for cls in classes:
        bpy.utils.unregister_class(cls)

    print("レベルエディタが無効化されました。")

#テスト実行用コード
if __name__ == "__main__":
    register()