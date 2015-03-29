#pragma once

class CBaseCharInfo
{
protected:
	UINT64			m_ModifyFlag;

	UINT64			m_CharID;
	BYTE			m_CharType;
	CEasyString		m_CharName;

	BYTE			m_Race;
	BYTE			m_Gender;

	UINT			m_CreatureDisplayID;
	
	BYTE			m_CharSkinColor;
	BYTE			m_CharHairColor;
	BYTE			m_CharFaceType;
	BYTE			m_CharHairType;
	BYTE			m_CharBeardType;
	bool			m_IsCharBald;

	UINT			m_MapID;

public:
	CBaseCharInfo(void);
	~CBaseCharInfo(void);

	virtual void Clear();

	void SetCharID(UINT64 Value);
	void SetCharType(BYTE Value);
	void SetCharName(LPCTSTR Value);
	void SetRace(BYTE Value);
	void SetGender(BYTE Value);
	void SetCreatureDisplayID(UINT Value);
	void SetCharSkinColor(BYTE Value);
	void SetCharHairColor(BYTE Value);
	void SetCharFaceType(BYTE Value);
	void SetCharHairType(BYTE Value);
	void SetCharBeardType(BYTE Value);
	void SetCharBald(bool Value);
	void SetMapID(UINT Value);
	


	UINT64 GetCharID();
	BYTE GetCharType();
	LPCTSTR GetCharName();
	BYTE GetRace();
	BYTE GetGender();
	UINT GetCreatureDisplayID();
	BYTE GetCharSkinColor();
	BYTE GetCharHairColor();
	BYTE GetCharFaceType();
	BYTE GetCharHairType();
	BYTE GetCharBeardType();
	bool GetIsCharBald();
	UINT GetMapID();

	virtual void GetModifyFlag(OBJECT_MEMBER_FLAGS& ModifyFlags,int GetType,const OBJECT_MEMBER_FLAGS& MemberFlags);
	virtual bool IsModified(const OBJECT_MEMBER_FLAGS& MemberFlags);
	virtual void ClearModifyFlag(const OBJECT_MEMBER_FLAGS& MemberFlags);

	virtual bool MakePacket(CSmartStruct& Packet,const OBJECT_MEMBER_FLAGS& MemberFlags);
	virtual bool MakeUpdatePacket(CSmartStruct& Packet,const OBJECT_MEMBER_FLAGS& MemberFlags);

	virtual bool ParsePacket(CSmartStruct& Packet,const OBJECT_MEMBER_FLAGS& MemberFlags);

};


inline void CBaseCharInfo::SetCharID(UINT64 Value)
{
	m_CharID=Value;
	m_ModifyFlag|=MF_BCI_CHAR_ID;
}
inline void CBaseCharInfo::SetCharType(BYTE Value)
{
	m_CharType=Value;
	m_ModifyFlag|=MF_BCI_CHAR_TYPE;
}
inline void CBaseCharInfo::SetCharName(LPCTSTR Value)
{
	m_CharName=Value;
	m_ModifyFlag|=MF_BCI_CHAR_NAME;
}

inline void CBaseCharInfo::SetRace(BYTE Value)
{
	m_Race=Value;
	m_ModifyFlag|=MF_BCI_RACE;
}
inline void CBaseCharInfo::SetGender(BYTE Value)
{
	m_Gender=Value;
	m_ModifyFlag|=MF_BCI_GENDER;
}
inline void CBaseCharInfo::SetCreatureDisplayID(UINT Value)
{
	m_CreatureDisplayID=Value;
	m_ModifyFlag|=MF_BCI_CREATURE_DISPLAY_ID;
}
inline void CBaseCharInfo::SetCharSkinColor(BYTE Value)
{
	m_CharSkinColor=Value;
	m_ModifyFlag|=MF_BCI_CHAR_SKIN_COLOR;
}
inline void CBaseCharInfo::SetCharHairColor(BYTE Value)
{
	m_CharHairColor=Value;
	m_ModifyFlag|=MF_BCI_CHAR_HAIR_COLOR;
}
inline void CBaseCharInfo::SetCharFaceType(BYTE Value)
{
	m_CharFaceType=Value;
	m_ModifyFlag|=MF_BCI_CHAR_FACE_TYPE;
}
inline void CBaseCharInfo::SetCharHairType(BYTE Value)
{
	m_CharHairType=Value;
	m_ModifyFlag|=MF_BCI_CHAR_HAIR_TYPE;
}
inline void CBaseCharInfo::SetCharBeardType(BYTE Value)
{
	m_CharBeardType=Value;
	m_ModifyFlag|=MF_BCI_CHAR_BEARD_TYPE;
}
inline void CBaseCharInfo::SetCharBald(bool Value)
{
	m_IsCharBald=Value;
	m_ModifyFlag|=MF_BCI_IS_CHAR_BALD;
}
inline void CBaseCharInfo::SetMapID(UINT Value)
{
	m_MapID=Value;
	m_ModifyFlag|=MF_BCI_MAP_ID;
}

inline UINT64 CBaseCharInfo::GetCharID()
{
	return m_CharID;
}
inline BYTE CBaseCharInfo::GetCharType()
{
	return m_CharType;
}
inline LPCTSTR CBaseCharInfo::GetCharName()
{
	return m_CharName;
}
inline BYTE CBaseCharInfo::GetRace()
{
	return m_Race;
}
inline BYTE CBaseCharInfo::GetGender()
{
	return m_Gender;
}
inline UINT CBaseCharInfo::GetCreatureDisplayID()
{
	return m_CreatureDisplayID;
}
inline BYTE CBaseCharInfo::GetCharSkinColor()
{
	return m_CharSkinColor;
}
inline BYTE CBaseCharInfo::GetCharHairColor()
{
	return m_CharHairColor;
}
inline BYTE CBaseCharInfo::GetCharFaceType()
{
	return m_CharFaceType;
}
inline BYTE CBaseCharInfo::GetCharHairType()
{
	return m_CharHairType;
}
inline BYTE CBaseCharInfo::GetCharBeardType()
{
	return m_CharBeardType;
}
inline bool CBaseCharInfo::GetIsCharBald()
{
	return m_IsCharBald;
}
inline UINT CBaseCharInfo::GetMapID()
{
	return m_MapID;
}