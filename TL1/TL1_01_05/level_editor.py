import bpy

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
    bl_idname = "TOPBAR_NT_my_menu"
    bl_label = "MyMenu"
    bl_description = "拡張メニュー by " + bl_info["author"]

    #サブメニューの描画
    def draw(self, context):
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


#Blenderに登録するクラスリスト
classes = (
    MYADDON_OT_create_ico_sphere,
    MYADDON_OT_streatch_vertex,
    TOPBAR_MT_my_menu,
)

# メニュー項目画面
def draw_menu_manual(self, context):
    self.layout.operator("wm.url_open_preset", text="Manual", icon = 'HELP')

#アドオン有効化時のこーるバック
def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)
    print("レベルエディタが有効化されました。")

#アドオン無効化時コールバック
def unregister():
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)

    for cls in classes:
        bpy.utils.unregister_class(cls)

    print("レベルエディタが無効化されました。")

#テスト実行用コード
if __name__ == "__main__":
    register()