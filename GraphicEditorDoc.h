
// GraphicEditorDoc.h: CGraphicEditorDoc 类的接口
//


#pragma once

#include <afxcoll.h>


class CGraphicEditorDoc : public CDocument
{
protected: // 仅从序列化创建
	CGraphicEditorDoc() noexcept;
	DECLARE_DYNCREATE(CGraphicEditorDoc)

// 特性
public:

// 操作
public:
	bool AddShape(CShape* pShape);             // 添加图形
    bool RemoveShapeAt(int index);             // Remove shape by index
	bool RemoveAllShapes();                    // 移除所有图形
	bool SortShapes();                         // 按照 z 坐标排序
	const CObArray& GetShapes() const;         // 获取图形列表
    bool AddCombination(CShape* pComb);        // 添加组合
    bool RemoveCombinationAt(int index);       // 移除组合
    bool RemoveAllCombinations();              // 清空组合
    const CObArray& GetCombinations() const;   // 获取所有组合
    int GetIndex(const CShape* pShape,         // Get index by the value of the
                 const CObArray& arr) const;   // CShape pointer.    

protected:
    static bool CmpByz(const void* elem1, const void* elem2);

// 重写
public:
	virtual BOOL OnNewDocument() override;
	virtual void Serialize(CArchive& ar) override;
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent() override;
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds) override;
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CGraphicEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    // Store all shapes.
    CObArray m_shapes;
    CObArray m_combinations;

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
