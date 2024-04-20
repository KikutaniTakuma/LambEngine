#include "Skeleton.h"
#include "GraphicsStructs.h"

#ifdef _DEBUG
#include "Drawers/Line/Line.h"
#endif // _DEBUG


Skeleton CreateSkeleton(const Node& rootNode)
{
    Skeleton skeleton;
    skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

    for (const Joint& joint : skeleton.joints) {
        skeleton.jointMap.emplace(joint.name, joint.index);
    }

    skeleton.Update();

    return skeleton;
}

int32_t CreateJoint(const Node& rootNode, const std::optional<int32_t> parent, std::vector<Joint>& joints)
{
    Joint joint;
    joint.name = rootNode.name;
    joint.localMatrix = rootNode.loacalMatrix;
    joint.skeletonSpaceMatrix = Mat4x4::kIdentity;
    joint.transform = rootNode.transform;
    joint.index = static_cast<int32_t>(joints.size());
    joint.parent = parent;
    joints.push_back(joint);
    for(const Node& child : rootNode.children) {
        int32_t childIndex = CreateJoint(child, joint.index, joints);
        joints[joint.index].children.push_back(childIndex);
    }
    return joint.index;
}

void Skeleton::Update() {
    for (Joint& joint : joints) {
        joint.localMatrix = joint.transform.GetMatrix();
        if (joint.parent) {
            joint.skeletonSpaceMatrix = joint.localMatrix * joints[*joint.parent].skeletonSpaceMatrix;
        }
        else {
            joint.skeletonSpaceMatrix = joint.localMatrix;
        }
    }
}

void Skeleton::Draw([[maybe_unused]]const Mat4x4& camera) {
#ifdef _DEBUG
    for (size_t i = 0; i < joints.size() - 1; i++) {
        //Vector3&& start = joints[i].skeletonSpaceMatrix.GetTranslate();
        //Vector3&& end = joints[i + 1].skeletonSpaceMatrix.GetTranslate();


    }
#endif // _DEBUG
}
