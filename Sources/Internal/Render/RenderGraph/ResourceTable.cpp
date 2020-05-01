#include "stdafx.h"

#include "Render/RenderGraph/ResourceTable.h"
#include "Render/Texture/Texture.h"

namespace Kioto::Renderer
{

ResourceTable::ResourceTable()
{
    m_blackboardsPool.resize(512);
}

ResourceTable::~ResourceTable()
{
    for (auto& it : m_resources)
        SafeDelete(it.second);
}

PassBlackboard* ResourceTable::GetNextBlackboard()
{
    return &m_blackboardsPool[m_currIndex++];
}

void ResourceTable::ClearBlackboards()
{
    for (uint32 i = 0; i < m_currIndex; ++i)
    {
        m_blackboardsPool[i].first = nullptr;
        m_blackboardsPool[i].second.Clear();
    }
    m_currIndex = 0;
}

ResourcesBlackboard* ResourceTable::GetBalackboardForPass(const RenderPass* pass)
{
    auto& it = std::find_if(m_blackboardsPool.begin(), m_blackboardsPool.end(),
        [&pass](const PassBlackboard& bb) { return bb.first == pass; });

    assert(it != m_blackboardsPool.end() && "Pass is unregistered for current frame");
    return &it->second;
}

}