#include "Skeleton.h"

#ifdef _DEBUG
#include "Drawers/Line/Line.h"
#endif // _DEBUG

namespace Lamb {
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
        for (const Node& child : rootNode.children) {
            int32_t childIndex = CreateJoint(child, joint.index, joints);
            joints[joint.index].children.push_back(childIndex);
        }
        return joint.index;
    }
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
    for (Joint& joint : joints) {
        if (not joint.parent) {
            continue;
        }
        Vector3&& start = joint.skeletonSpaceMatrix.GetTranslate();
        Vector3&& end = joints[*joint.parent].skeletonSpaceMatrix.GetTranslate();

        Line::Draw(
            start,
            end,
            camera,
            std::numeric_limits<uint32_t>::max()
        );
    }
#endif // _DEBUG
}
