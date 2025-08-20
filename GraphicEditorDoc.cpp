
// GraphicEditorDoc.cpp: CGraphicEditorDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "GraphicEditor.h"
#endif

#include "GraphicEditorDoc.h"

#include <propkey.h>
#include <algorithm>
#include <vector>

#include "Shapes/Shape.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGraphicEditorDoc

IMPLEMENT_DYNCREATE(CGraphicEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CGraphicEditorDoc, CDocument)
END_MESSAGE_MAP()


// CGraphicEditorDoc 构造/析构

CGraphicEditorDoc::CGraphicEditorDoc() noexcept
{
	// TODO: 在此添加一次性构造代码
    m_combinations.RemoveAll();
    m_shapes.RemoveAll();
}

CGraphicEditorDoc::~CGraphicEditorDoc()
{
    int count = m_shapes.GetCount();
    for (int i = count - 1; i >= 0; i--)  // 反向遍历更安全
    {
        if (m_shapes[i] != nullptr)
        {
            delete m_shapes[i];
            m_shapes.SetAt(i, nullptr);  // 设置为nullptr
        }
    }
    m_shapes.RemoveAll();

    count = m_combinations.GetCount();
    for (int i = count - 1; i >= 0; i--)  // 反向遍历更安全
    {
        if (m_combinations[i] != nullptr)
        {
            delete m_shapes[i];
            m_combinations.SetAt(i, nullptr);  // 设置为nullptr
        }
    }
    m_combinations.RemoveAll();
}


bool CGraphicEditorDoc::AddShape(CShape* pShape) {
    if (pShape == nullptr) {
        return false;
    }
    m_shapes.Add(pShape);
    SetModifiedFlag(TRUE);  // 标记文档已修改
    // We use Invalidate() in every View.
    // UpdateAllViews(NULL);   // 更新所有视图
    return true;
}

int CGraphicEditorDoc::GetIndex(const CShape* pShape, const CObArray& arr) const
{
    if (pShape == nullptr)
        return -1;

    // 遍历形状数组查找匹配指针
    for (int i = 0; i < arr.GetCount(); i++)
    {
        if (arr[i] == pShape)  // 直接比较指针地址
        {
            return i;
        }
    }

    return -1;  // 未找到
}

bool CGraphicEditorDoc::RemoveShapeAt(int index)
{
    if (index >= 0 && index < m_shapes.GetCount())
    {
        delete m_shapes[index];  // 释放内存
        m_shapes.RemoveAt(index);
        SetModifiedFlag(TRUE);
        return true;
    }
    return false;
}                                       // Remove shape by index

bool CGraphicEditorDoc::RemoveAllShapes() {
    const int count = m_shapes.GetCount();
    for (int i = count - 1; i >= 0; i--)  // 反向遍历更安全
    {
        if (m_shapes[i] != nullptr)
        {
            delete m_shapes[i];
            m_shapes.SetAt(i, nullptr);  // 设置为nullptr
        }
    }
    m_shapes.RemoveAll();
    SetModifiedFlag(TRUE);
    // Unnecessary Updation
    // UpdateAllViews(NULL);
    return true;
}
bool CGraphicEditorDoc::SortShapes() {
    int nCount = m_shapes.GetCount();
    if (nCount < 2) return true;

    // 复制到临时vector
    std::vector<CShape*> temp(nCount);
    for (int i = 0; i < nCount; ++i) {
        temp[i] = dynamic_cast<CShape*>(m_shapes[i]);
    }

    // 排序
    std::sort(temp.begin(), temp.end(), CmpByz);

    // 复制回CObArray
    m_shapes.RemoveAll();
    for (auto pShape : temp) {
        m_shapes.Add(pShape);
    }

    // Unnecessary Updation
    // UpdateAllViews(NULL);
    return true;
}

bool CGraphicEditorDoc::CmpByz(const void* elem1, const void* elem2) {
    // 转换指针类型
    CShape* pShape1 = *(CShape**)elem1;
    CShape* pShape2 = *(CShape**)elem2;

    return pShape1->getZ() <= pShape1->getZ();
}

const CObArray& CGraphicEditorDoc::GetShapes() const {
    return m_shapes;
}

bool CGraphicEditorDoc::AddCombination(CShape* pComb) {
    if (pComb == nullptr) {
        return false;
    }
    m_combinations.Add(pComb);  // 添加组合
    SetModifiedFlag(TRUE);  // 标记文档已修改
    return true;
}

bool CGraphicEditorDoc::RemoveCombinationAt(int index) {
    if (index >= 0 && index < m_combinations.GetCount()) {
        delete m_combinations[index];  // 释放内存
        m_combinations.RemoveAt(index);
        SetModifiedFlag(TRUE);  // 标记文档已修改
        return true;
    }
    return false;
}
bool CGraphicEditorDoc::RemoveAllCombinations() {
    const int count = m_combinations.GetCount();
    for (int i = count - 1; i >= 0; i--)  // 反向遍历更安全
    {
        if (m_combinations[i] != nullptr)
        {
            delete m_combinations[i];
            m_combinations.SetAt(i, nullptr);  // 设置为nullptr
        }
    }
    m_combinations.RemoveAll();
    SetModifiedFlag(TRUE);  // 标记文档已修改
    return true;
}
const CObArray& CGraphicEditorDoc::GetCombinations() const {
    return m_combinations;
}

BOOL CGraphicEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CGraphicEditorDoc 序列化

void CGraphicEditorDoc::Serialize(CArchive& ar)
{
    // 首先调用基类序列化
    CDocument::Serialize(ar);

    if (ar.IsStoring()) {
        // 存储数据时的逻辑

        // 可以添加其他需要存储的数据
        // ar << m_someOtherData;

        // CObArray会自动序列化其内容
        m_shapes.Serialize(ar);
        m_combinations.Serialize(ar);
    } else {
        // 加载数据时的逻辑

        // 先清空现有数据
        RemoveAllShapes();
        RemoveAllCombinations();

        // CObArray会自动反序列化其内容
        m_shapes.Serialize(ar);
        m_combinations.Serialize(ar);

        // 可以添加其他需要加载的数据
        // ar >> m_someOtherData;

        // 更新视图
        UpdateAllViews(nullptr);
    }
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CGraphicEditorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CGraphicEditorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CGraphicEditorDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CGraphicEditorDoc 诊断

#ifdef _DEBUG
void CGraphicEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGraphicEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGraphicEditorDoc 命令
