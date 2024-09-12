//pugxml.h
///////////////////////////////////////////////////////////////////////////////
//
// Pug XML Parser - Version 1.0002
// --------------------------------------------------------
// Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
// Released into the Public Domain. Use at your own risk.
// See pugxml.xml for further information, history, etc.
// Contributions by Neville Franks (readonly@getsoft.com).
//
// NF 29 Mar 2003  Added xml_attribute_struct::matches_attribute_name_value() and
//                 changed xml_node::matches_attribute_name_value() to use same.
// NF 29 Mar 2003  Changed xml_attribute_struct::matches_attribute_name_value() to
//                 compare value lengths instead of using _tcsncmp(,,Max(..)) which
//                 doesn't work when str1 is shorter than str2, but otherwise match.
// NF 31 Mar 2003  Added findattribute( const TCHAR* name, const TCHAR* value ).
// NF 31 Mar 2003  Changed first_xx functions to const.
// NF 31 Mar 2003  Added CEasyString xml_to_text( const CEasyString& sxml_pcdata ).
// NF 17 Apr 2003  Added visible_children to xml_node_struct.
// NF 29 May 2003  Changed static pug::parse() to xml_parser::parse()
// NF 29 May 2003  Added code to xml_parser::parse() to set node expand flag when
//                 attribute 'expanded="true"' is seen. Also adds node to new
//                 xml_parser::_expanded_nodes vector.
// NF 25 Jun 2003  Added xml_node_struct* find_child( const xml_node_type type ).
// NF 26 Jun 2003  Added xml_node_struct* find_child( const xml_node_struct* xns ).
// NF 26 Jun 2003  Added xml_node_struct* disconnect_child( const unsigned int i ).
// NF 26 Jun 2003  Changed remove_child() to use new disconnect_child().
// NF 26 Jun 2003  Updated insert_child() & append_node() to allow the child node
//                 to be specified instead of creating a new node.
// NF 26 Jun 2003  Updated insert_child() to only call append_node() and to set
//                 'p->name_insitu = p->value_insitu = false' which it previously
//                 only did when the new node was appended as the last child.
// NF 20 Jul 2003  Made some more functions const.
// NF 18 Sep 2003  Added xml_tree_walker.for_each( , bDescend ) param option.
// NF 21 Sep 2003  Added first_element_by_attribute(..., const bool bDescend=true) param.
// NF 7 Oct 2003   In both xml_node::first_element_by_value() and xml_node::first_element_by_attribute()
//                 I have removed an incorrect 'else' which prevented the functions
//                 from descending the tree.
// NF 7 Oct 2003   Applied fix to xml_node Copy constructor as per http://www.codeproject.com/soap/pugxml.asp?msg=609169#xx609169xx
//
// TODO:
//   P4 Change calls to _tcsncmp( ..., Max(l1,l2) ) to 'l1 == l2 && _tcsncmp( .., l1 )
//      It is more efficient to compare lengths first, than using Max() and  _tcsncmp().
//      See: matches_attribute_name_value()
//
//   PZ I'd like to see pcdata stored in the element node.value if possible, instead of
//      in a separate child node. It is quite confusing having it as a node in the DOM
//      AFAIC. However it needs to be a node so that it is ordered correctly in the DOM
//      ie. It isn't necessarrilly the first element.child node. 25 Jun 2003
//
///////////////////////////////////////////////////////////////////////////////


#define PUGAPI_VARIANT 0x584766ef	//The Pug XML library variant we are using in this implementation.
#define PUGAPI_VERSION_MAJOR 1		//The Pug XML library major version we are using in this implementation.
#define PUGAPI_VERSION_MINOR 2		//The Pug XML library minor version we are using in this implementation.
///
#if !defined(_PUGXML_)
#define _PUGXML_
#pragma once


#define PUGOPT_MEMFIL //Uncomment to enable memory-mapped file parsing support.
//#define PUGOPT_NONSEG //Uncomment to enable non-destructive (non-segmenting) parsing support.


#ifdef PUGOPT_MEMFIL
#	ifndef PUGOPT_NONSEG
#		define PUGOPT_NONSEG //PUGOPT_MEMFIL implies PUGOPT_NONSEG.
#	endif
#endif




#ifndef HIWORD
#	define HIWORD(X) ((unsigned short)((unsigned int)(X)>>16))
#	define LOWORD(X) ((unsigned short)((unsigned int)(X)&0xFFFF))
#endif


//<summary>
//	Library variant ID. The ID 0x58475550 is owned by Kristen Wegner. You *MUST*
//	provide your own unique ID if you modify or fork the code in this library to
//	your own purposes. If you change this then *you* are now the maintainer, not me.
//	Change also in the package section of pugxml.xml, and append yourself to the
//	authors section.
//</summary>
#define PUGAPI_INTERNAL_VARIANT 0x584766ef  // NF. 28 Mar 2003 0x584766ef is owned by Neville Franks readonly@getsoft.com
//<summary>Major version. Increment for each major release. Only change if you own the variant.</summary>
#define PUGAPI_INTERNAL_VERSION_MAJOR 1
//<summary>Minor version. Increment for each minor release. Only change if you own the variant ID.</summary>
#define PUGAPI_INTERNAL_VERSION_MINOR 2

#define PUGAPI_INTERNAL_VERSION ((PUGAPI_INTERNAL_VERSION_MINOR&0xFFFF)|PUGAPI_INTERNAL_VERSION_MAJOR<<16)


#define PUGDEF_ATTR_NAME_SIZE 128
#define PUGDEF_ATTR_VALU_SIZE 256
#define PUGDEF_ELEM_NAME_SIZE 256


//<summary>The PugXML Parser namespace.</summary>
namespace pug
{

//<summary>The Library Variant ID. See PUGAPI_INTERNAL_VARIANT for an explanation.</summary>
//<returns>The current Library Variant ID.</returns>
inline static unsigned int lib_variant(){ return PUGAPI_INTERNAL_VARIANT; }
//<summary>The library version. High word is major version. Low word is minor version.</summary>
//<returns>The current Library Version.</returns>
inline static unsigned int lib_version(){ return PUGAPI_INTERNAL_VERSION; }

#define Min(a,b)	(a<b?a:b)
#define Max(a,b)	(a>b?a:b)


//<summary>A 'name=value' XML attribute structure.</summary>
typedef struct t_xml_attribute_struct
{
	TCHAR*			name;			//Pointer to attribute name.
	bool			name_insitu;	//True if 'name' is a segment of the original parse string.
#ifdef PUGOPT_NONSEG
	unsigned int	name_size;		//Length of element name.
#endif
	TCHAR*			value;			//Pointer to attribute value.
	bool			value_insitu;	//True if 'value' is a segment of the original parse string.
#ifdef PUGOPT_NONSEG
	unsigned int	value_size;		//Length of element name.
#endif

#ifdef PUGOPT_NONSEG
	inline bool matches_attribute_name_value(const TCHAR* pvalue,const unsigned int valuelen) const { return (valuelen==value_size && _tcsncmp(pvalue,value,value_size)==0); } // This attribute is valued 'value'. NF 28 Mar 2003
#else
	inline bool matches_attribute_name_value(const TCHAR* pvalue) const { return (strcmpwild(pvalue,value)==0); }  // This attribute is valued 'value'. NF 28 Mar 2003
#endif
}
	xml_attribute_struct;


//<summary>Tree node classification.</summary>
//<remarks>See 'xml_node_struct::type'.</remarks>
typedef enum t_xml_node_type
{
	node_null,			//An undifferentiated entity.
	node_document,		//A document tree's absolute root.
	node_element,		//E.g. '&lt;...&gt;'
	node_pcdata,		//E.g. '&gt;...&lt;'
	node_cdata,			//E.g. '&lt;![CDATA[...]]&gt;'
	node_comment,		//E.g. '&lt;!--...--&gt;'
	node_pi,			//E.g. '&lt;?...?&gt;'
	node_include,		//E.g. '&lt;![INCLUDE[...]]&gt;'
	node_doctype,		//E.g. '&lt;!DOCTYPE ...&gt;'.
	node_dtd_entity,	//E.g. '&lt;!ENTITY ...&gt;'.
	node_dtd_attlist,	//E.g. '&lt;!ATTLIST ...&gt;'.
	node_dtd_element,	//E.g. '&lt;!ELEMENT ...&gt;'.
	node_dtd_notation	//E.g. '&lt;!NOTATION ...&gt;'.
}
	xml_node_type;


static const unsigned int parse_grow = 4; //Default child element & attribute space growth increment.


//Parser Options
static const unsigned int parse_minimal			= 0x00000000; //Unset the following flags.
static const unsigned int parse_pi				= 0x00000002; //Parse '&lt;?...?&gt;'
static const unsigned int parse_doctype			= 0x00000004; //Parse '&lt;!DOCTYPE ...&gt;' section, setting '[...]' as data member.
static const unsigned int parse_comments		= 0x00000008; //Parse &lt;!--...--&gt;'
static const unsigned int parse_cdata			= 0x00000010; //Parse '&lt;![CDATA[...]]&gt;', and/or '&lt;![INCLUDE[...]]&gt;'
static const unsigned int parse_escapes			= 0x00000020; //Not implemented.
static const unsigned int parse_trim_pcdata		= 0x00000040; //Trim '&gt;...&lt;'
static const unsigned int parse_trim_attribute	= 0x00000080; //Trim 'foo="..."'.
static const unsigned int parse_trim_cdata		= 0x00000100; //Trim '&lt;![CDATA[...]]&gt;', and/or '&lt;![INCLUDE[...]]&gt;'
static const unsigned int parse_trim_entity		= 0x00000200; //Trim '&lt;!ENTITY name ...&gt;', etc.
static const unsigned int parse_trim_doctype	= 0x00000400; //Trim '&lt;!DOCTYPE [...]&gt;'
static const unsigned int parse_trim_comment	= 0x00000800; //Trim &lt;!--...--&gt;'
static const unsigned int parse_wnorm			= 0x00001000; //Normalize all entities that are flagged to be trimmed.
static const unsigned int parse_dtd				= 0x00002000; //If parse_doctype set, then parse whatever is in data member ('[...]').
static const unsigned int parse_dtd_only		= 0x00004000; //If parse_doctype|parse_dtd set, then parse only '&lt;!DOCTYPE [*]&gt;'
static const unsigned int parse_default			= 0x0000FFFF;
static const unsigned int parse_noset			= 0x80000000;

#ifdef PUGOPT_NODE_FLAGS
// xml_node_struct state flags
static const unsigned int  node_expanded    = 0x00000001;   // the node is expanded
static const unsigned int  node_selected    = 0x00000002;   // the node is selected
static const unsigned int  node_display_root = 0x00000004;  // the first node to display. ie. the display root. @since 7 Aug 2003
#endif

//<summary>An XML document tree node.</summary>
typedef struct t_xml_node_struct
{
	t_xml_node_struct*		parent;				//Pointer to parent
	TCHAR*					name;				//Pointer to element name.
#ifdef PUGOPT_NONSEG
	unsigned int			name_size;			//Length of element name. Since 19 Jan 2003 NF.
#endif
	bool					name_insitu;		//True if 'name' is a segment of the original parse string.
	xml_node_type			type;				//Node type; see xml_node_type.
	unsigned int			attributes;			//Count attributes.
	unsigned int			attribute_space;	//Available pointer space in 'attribute'.
	xml_attribute_struct**	attribute;			//Array of pointers to attributes; see xml_attribute_struct.
	unsigned int			children;			//Count children in member 'child'.
	unsigned int			child_space;		//Available pointer space in 'child'.
	t_xml_node_struct**		child;				//Array of pointers to children.
	TCHAR*					value;				//Pointer to any associated string data.
#ifdef PUGOPT_NONSEG
	unsigned int			value_size;			//Length of element data. Since 19 Jan 2003 NF.
#endif
	bool					value_insitu;		//True if 'data' is a segment of the original parse string.
#ifdef PUGOPT_NODE_FLAGS
	unsigned int			flags;	            // node_expanded, node_selected, node_display_root. NF 19 Mar 2003
	unsigned int			visible_children;	// count of visible children, including itself. NF 17 Apr 2003
#endif

            // NF 10 Mar 2003
	inline bool type_document() const { return (this == parent && type == node_document); } // Node is tree root.
	inline bool type_element() const { return (type == node_element); } // Node is an element. @since 25 Jun 2003
	inline bool type_pcdata() const { return (type == node_pcdata); } // Node is PCDATA. @since 25 Jun 2003
	inline unsigned int childcount() const { return this->children; } // Access node's child count.
	inline t_xml_node_struct* getparent() const { return parent; } // Access parent
	inline t_xml_node_struct* get_parent() const { return type_document() ? NULL : parent; } // Access parent, return NULL at root. NF 17 Apr 2003
	t_xml_node_struct* getchild(unsigned int i) const { return (i < children) ? child[i] : NULL; } //Access child node struct at subscript
	t_xml_node_struct* firstchild() const { return children ? child[0] : NULL; } //Access first child node struct
	t_xml_node_struct* lastchild() const { return children ? child[children-1] : NULL; } //Access last child node struct
#ifdef PUGOPT_NODE_FLAGS
	inline bool is_expanded() const { return (flags & node_expanded) != 0; } // Node is expanded.
	inline bool is_selected() const { return (flags & node_selected) != 0; } // Node is selected.
	inline bool is_display_root() const { return (flags & node_display_root) != 0; } // Node is the display root.
	inline void expand( const bool bexpand ) { if ( bexpand ) (flags |= node_expanded); else (flags &= ~node_expanded); } // set/reset the node is expanded state.
	inline void select( const bool bselect ) { if ( bselect ) (flags |= node_selected); else (flags &= ~node_selected); } // set/reset the node is selected state.
	inline void set_display_root( const bool bset ) { if ( bset ) (flags |= node_display_root); else (flags &= ~node_display_root); } // set/reset the node is display root state.
	inline unsigned int get_visible_children() const { return visible_children; }	// count of visible children, including itself.
	inline unsigned int set_visible_children( const unsigned int _visible_children ) { return visible_children = _visible_children; }
	inline unsigned int inc_visible_children( const unsigned int _visible_children ) { return visible_children += _visible_children; }
#endif
}
xml_node_struct;


//<summary>Concatenate 'rhs' to 'lhs', growing 'rhs' if neccessary.</summary>
//<param name="lhs">Pointer to pointer to receiving string. Note: If '*lhs' is not null, it must have been dynamically allocated using 'malloc'.</param>
//<param name="rhs">Source.</param>
//<returns>Success if 'realloc' was successful.</returns>
//<remarks>'rhs' is resized and 'rhs' is concatenated to it.</remarks>
inline static bool strcatgrow(TCHAR** lhs,const TCHAR* rhs)
{
	if(!*lhs) //Null, so first allocate.
	{
		*lhs = (TCHAR*)MONITORED_MALLOC(_T("pugxml"), 1UL*sizeof(TCHAR));
		**lhs = 0; //Zero-terminate.
	}
	size_t ulhs = _tcslen(*lhs);
	size_t urhs = _tcslen(rhs);
	TCHAR* temp = (TCHAR*)MONITORED_REALLOC(_T("pugxml"), *lhs, (ulhs + urhs + 1UL) * sizeof(TCHAR));
	if(!temp) return false; //Realloc failed.
	memcpy(temp+ulhs,rhs,urhs*sizeof(TCHAR)); //Concatenate.
	temp[ulhs+urhs] = 0; //Terminate it.
	*lhs = temp;
	return true;
}

inline CEasyString XMLEncodeString(LPCTSTR Str)
{
	CEasyString OutStr(Str);
	OutStr.Replace(_T("&"), _T("&amp;"));
	OutStr.Replace(_T("<"), _T("&lt;"));
	OutStr.Replace(_T(">"), _T("&gt;"));
	OutStr.Replace(_T("\""), _T("&quot;"));
	OutStr.Replace(_T("\r"), _T("&#13;"));
	OutStr.Replace(_T("\n"), _T("&#10;"));
	OutStr.Replace(_T("	"), _T("&tab;"));
	return OutStr;
}
inline void XMLDecodeString(CEasyString& Str)
{
	Str.Replace(_T("&lt;"), _T("<"));
	Str.Replace(_T("&gt;"), _T(">"));
	Str.Replace(_T("&quot;"), _T("\""));
	Str.Replace(_T("&rt;"), _T("\r"));
	Str.Replace(_T("&nl;"), _T("\n"));
	Str.Replace(_T("&#13;"), _T("\r"));
	Str.Replace(_T("&#10;"), _T("\n"));
	Str.Replace(_T("&tab;"), _T("	"));
	Str.Replace(_T("&amp;"), _T("&"));
}

inline static bool chartype_symbol(TCHAR c) //Character is alphanumeric, -or- '_', -or- ':', -or- '-', -or- '.'.
{ return (_istalnum(c)||c==_T('_')||c==_T(':')||c==_T('-')||c==_T('.')); }
inline static bool chartype_space(TCHAR c) //Character is greater than 0 or character is less than exclamation.
{ return (c>0 && c<_T('!')); }
inline static bool chartype_enter(TCHAR c) //Character is '&lt;'.
{ return (c==_T('<')); }
inline static bool chartype_leave(TCHAR c) //Character is '&gt;'.
{ return (c==_T('>')); }
inline static bool chartype_close(TCHAR c) //Character is '/'.
{ return (c==_T('/')); }
inline static bool chartype_equals(TCHAR c) //Character is '='.
{ return (c==_T('=')); }
inline static bool chartype_special(TCHAR c) //Character is '!'.
{ return (c==_T('!')); }
inline static bool chartype_pi(TCHAR c) //Character is '?'.
{ return (c==_T('?')); }
inline static bool chartype_dash(TCHAR c) //Character is '-'.
{ return (c==_T('-')); }
inline static bool chartype_quote(TCHAR c) //Character is &quot;&lsquo;&quot; -or- &lsquo;&quot;&lsquo;.
{ return (c==_T('"')||c==_T('\'')); }
inline static bool chartype_lbracket(TCHAR c) //Character is '['.
{ return (c==_T('[')); }
inline static bool chartype_rbracket(TCHAR c) //Character is ']'.
{ return (c==_T(']')); }


#ifdef PUGOPT_NONSEG


//<summary>Concatenate 'rhs' to 'lhs', growing 'lhs' if neccessary.</summary>
//<param name="lhs">Pointer to pointer to receiving string. Note: If '*lhs' is not null, it must have been dynamically allocated using 'malloc'.</param>
//<param name="rhs">Source.</param>
//<param name="lsize">Specifies the length of *lhs in bytes and returns its new length.</param>
//<param name="rsize">Specifies the length of *rhs in bytes.</param>
//<returns>Success if 'realloc' was successful.</returns>
//<remarks>'lhs' is resized and 'rhs' is concatenated to it.</remarks>
inline static bool strcatgrown_impl(TCHAR** lhs,const TCHAR* rhs,unsigned int& lsize,unsigned int rsize)
{
	if(!*lhs) //Null, allocate and copy.
	{
		*lhs = (TCHAR*)MONITORED_MALLOC(_T("pugxml"), (rsize+1)*sizeof(TCHAR));
		if(!*lhs)
		{
			lsize = 0;
			return false; //Allocate failed.
		}
		memcpy(*lhs,rhs,rsize*sizeof(TCHAR)); //Concatenate.
		*(*lhs + rsize) = 0; //Terminate it.
		lsize = rsize;
	}
	else //Reallocate. NF I don't think this is right for MBCS, nor is code in 'StrCatGrow()'.
	{
		TCHAR* temp = (TCHAR*)MONITORED_REALLOC(_T("pugxml"), *lhs,(lsize + rsize + 1)*sizeof(TCHAR));
		if(!temp) return false; //Realloc failed.
		memcpy(temp+lsize,rhs,rsize*sizeof(TCHAR)); //Concatenate.
		lsize += rsize; //Set new length.
		temp[lsize] = 0; //Terminate it.
		*lhs = temp;
	}
	return true;
}

//<summary>Concatenate 'rhs' to 'lhs', growing 'lhs' if neccessary.</summary>
//<param name="lhs">Pointer to pointer to receiving string. Note: If '*lhs' is not null, it must have been dynamically allocated using 'malloc'.</param>
//<param name="rhs">Source.</param>
//<param name="lsize">Specifies the length of *lhs in bytes and returns its new length.</param>
//<returns>Success if 'realloc' was successful.</returns>
//<remarks>'lhs' is resized and 'rhs' is concatenated to it.</remarks>
inline static bool strcatgrown(TCHAR** lhs,const TCHAR* rhs,unsigned int& lsize)
{
	const unsigned int rsize = _tcslen(rhs);
	return pug::strcatgrown_impl(lhs,rhs,lsize,rsize);
}

//<summary>Trim leading and trailing whitespace.</summary>
//<param name="s">Pointer to pointer to string.</param>
//<param name="len">Specifies the length of *s in bytes and returns its new length.</param>
//<returns>Success.</returns>
//<remarks>*s is modified to point to the first non-white character in the string.</remarks>
inline static bool strwtrim(TCHAR** s,unsigned int& len)
{
	if(!s || !*s) return false;
	TCHAR* pse = *s + len;
	while(*s < pse && pug::chartype_space(**s)) //Find first non-white character.
		++*s; //As int as we hit whitespace, increment the string pointer.
	for(; *s < --pse;) //As int as we hit whitespace, decrement.
	{
		if(!pug::chartype_space(*pse))
		{
			len = pse + 1 - *s;
			break;
		}
	}
	return true;
}


#else


//<summary>Trim leading and trailing whitespace.</summary>
//<param name="s">Pointer to pointer to string.</param>
//<returns>Success.</returns>
inline static bool strwtrim(TCHAR** s)
{
	if(!s || !*s) return false;
	while(**s > 0 && **s < _T('!')) ++*s; //As int as we hit whitespace, increment the string pointer.
	const TCHAR* temp = *s;
	while(0 != *temp++); //Find the terminating null.
	int i, n = (int)(temp-*s-1);
	--n; //Start from the last string TCHAR.
	for(i=n; (i > -1) && (*s)[i] > 0 && (*s)[i] < _T('!'); --i); //As int as we hit whitespace, decrement.
	if(i<n) (*s)[i+1] = 0; //Zero-terminate.
	return true;
}


//<summary>
//	In situ trim leading and trailing whitespace, then convert all consecutive
//	whitespace to a single space TCHAR.
//</summary>
//<param name="s">Pointer to pointer to string.</param>
//<returns>Success.</returns>
inline static bool strwnorm(TCHAR** s)
{
	if(!s || !*s) return false; //No string to normalize.
	while(**s > 0 && **s < _T('!')) ++(*s); //As int as we hit whitespace, increment the string pointer.
	const TCHAR* temp = *s;
	while(0 != *temp++); //Find the terminating null.
	int n = (int)(temp-*s-1);
	TCHAR* norm = (TCHAR*)MONITORED_MALLOC(_T("pugxml"), sizeof(TCHAR)*(n+1)); //Allocate a temporary normalization buffer.
	if(!norm) return false; //Allocation failed.
	memset(norm,0,sizeof(TCHAR)*(n+1)); //Zero it.
	int j = 1;
	norm[0] = (*s)[0];
	for(int i=1; i<n; ++i) //For each character, starting at offset 1.
	{
		if((*s)[i] < _T('!')) //Whitespace-like.
		{
			if((*s)[i-1] >= _T('!')) //Previous was not whitespace-like.
			{
				norm[j] = _T(' '); //Convert to a space TCHAR.
				++j; //Normalization buffer grew by one TCHAR.
			}
		}
		else { norm[j] = (*s)[i]; ++j; } //Not whitespace, so just copy over.
	}
	if(j < n) //Normalization buffer is actually different that input.
	{
		_tcsncpy(*s,norm,j); //So, copy it back to input.
		(*s)[j] = 0; //Zero-terminate.
	}
	MONITORED_FREE(norm); //Don't need this anymore.
	--n; //Start from the last string TCHAR.
	for(i=n; (i > -1) && (*s)[i] > 0 && (*s)[i] < _T('!'); --i); //Find the first non-whitespace from the end.
	if(i<n) (*s)[i+1] = 0; //Truncate it.
	return true;

}

#endif


//<summary>Set structure string member to given value.</summary>
//<param name="dest">Pointer to pointer to destination.</param>
//<param name="src">Source.</param>
//<param name="insitu">Pointer to boolean in-situ string flag.</param>
//<returns>True if member was set to the new value.</returns>
//<remarks>
//	If 'src' is larger than 'dest' then 'dest' is resized, in which case
//	it is probably no longer in-situ,and 'in_situ' is set to false. If
//	'dest' is already no longer in-situ, and 'src' is too small then the
//	existing memory pointed to is freed. If 'dest' is larger than or equal
//	to 'dest' then it is merely copied with no resize.
//</remarks>
inline static bool strcpyinsitu
	(
		TCHAR**			dest,
		const TCHAR*	src,
		bool*			insitu
#ifdef PUGOPT_NONSEG
		,
		unsigned int&	destlen
#endif
	)
{
	if(!dest || !src || !insitu) return false; //Bad argument(s), so fail.
#ifndef PUGOPT_NONSEG //Always use heap for our r/o string.
	size_t l = (*dest) ? _tcslen(*dest) : 0; //How int is destination?
	if(l >= _tcslen(src)) //Destination is large enough, so just copy.
	{
		_tcscpy(*dest,src); //Copy.
		return TRUE; //Success.
	}
	else //Destination is too small.
#endif
	{
		if(*dest && !*insitu) MONITORED_FREE(*dest); //If destination is not in-situ, then free it.
		*dest = NULL; //Mark destination as NULL, forcing 'StrCatGrow' to 'malloc.
#ifdef PUGOPT_NONSEG
		if(strcatgrown(dest,src,destlen)) //Allocate & copy source to destination
#else
		if(strcatgrow(dest,src)) //Allocate & copy source to destination
#endif
		{
			*insitu = false; //Mark as no longer being in-situ, so we can free it later.
			return TRUE; //Success.
		}
	}
	return false; //Failure.
}


//<summary>Character set pattern match.</summary>
//<param name="lhs">String or expression for left-hand side of comparison.</param>
//<param name="rhs">String for right-hand side of comparison.</param>
//<remarks>Used by 'strcmpwild'.</remarks>
inline int strcmpwild_cset(const TCHAR** src,const TCHAR** dst)
{
	int find = 0;
	int excl = 0;
	int star = 1;
	if(**src == _T('!'))
	{
		excl = 1;
		++(*src);
	}
	while(**src != _T(']') || star == 1)
	{
		if(find == 0)
		{
			if(**src == _T('-') && *(*src-1) < *(*src+1) && *(*src+1) != _T(']') && star == 0)
			{
				if(**dst >= *(*src-1) && **dst <= *(*src+1))
				{
					find = 1;
					++(*src);
				}
			}
			else if(**src == **dst) find = 1;
		}
		++(*src);
		star = 0;
	}
	if(excl == 1) find = (1 - find);
	if(find == 1) ++(*dst);
	return find;
}


int strcmpwild_impl(const TCHAR* src,const TCHAR* dst); //Forward declaration.


//<summary>Wildcard pattern match.</summary>
//<param name="lhs">String or expression for left-hand side of comparison.</param>
//<param name="rhs">String for right-hand side of comparison.</param>
//<remarks>Used by 'strcmpwild'.</remarks>
inline int strcmpwild_astr(const TCHAR** src,const TCHAR** dst)
{
	int find = 1;
	++(*src);
	while((**dst != 0 && **src == _T('?')) || **src == _T('*'))
	{
		if(**src == _T('?')) ++(*dst);
		++(*src);
	}
	while(**src == _T('*')) ++(*src);
	if(**dst == 0 && **src != 0) return 0;
	if(**dst == 0 && **src == 0) return 1;
	else
	{
		if(strcmpwild_impl(*src,*dst) == 0)
		{
			do
			{
				++(*dst);
				while(**src != **dst && **src != _T('[') && **dst != 0)
					++(*dst);
			}
			while((**dst != 0) ? strcmpwild_impl(*src,*dst) == 0 : 0 != (find=0));
		}
		if(**dst == 0 && **src == 0) find = 1;
		return find;
	}
}


//<summary>Compare two strings, with globbing, and character sets.</summary>
//<param name="lhs">String or expression for left-hand side of comparison.</param>
//<param name="rhs">String for right-hand side of comparison.</param>
//<remarks>Used by 'strcmpwild'.</remarks>
inline int strcmpwild_impl(const TCHAR* src,const TCHAR* dst)
{
	int find = 1;
	for(; *src != 0 && find == 1 && *dst != 0; ++src)
	{
		switch(*src)
		{
		case _T('?'): ++dst; break;
		case _T('['): ++src; find = strcmpwild_cset(&src,&dst); break;
		case _T('*'): find = strcmpwild_astr(&src,&dst); --src; break;
		default : find = (int) (*src == *dst); ++dst;
		}
	}
	while(*src == _T('*') && find == 1) ++src;
	return (int) (find == 1 && *dst == 0 && *src == 0);
}

//<summary>Compare two strings, with globbing, and character sets.</summary>
//<param name="lhs">String or expression for left-hand side of comparison.</param>
//<param name="rhs">String for right-hand side of comparison.</param>
//<returns>
//	Returns 1 if src does not match dst, or -1 if either src or dst are null,
//	or 0 if src matches dst.
//</returns>
//<remarks>
//	Simple regular expressions are permitted in 'src': The character '*' matches
//	zero or more characters up to the next pattern, or the end of the string. The
//	'?' character matches any single character. Character sets and negation are
//	also permitted, for example, '[abcd]', '[a-zA-Z]', etc.
//</remarks>
inline int strcmpwild(const TCHAR* src,const TCHAR* dst)
{
	if(!src || !dst) return -1;
	return (strcmpwild_impl(src,dst)==1)?0:1;
}


//<summary>Allocate & init an xml_attribute_struct structure.</summary>
//<returns>Pointer to new xml_attribute_struct structure.</returns>
inline static xml_attribute_struct* new_attribute(void)
{
	xml_attribute_struct* p = (xml_attribute_struct*)MONITORED_MALLOC(_T("pugxml"), sizeof(xml_attribute_struct)); //Allocate one attribute.
	if(p) //If allocation succeeded.
	{
		p->name = p->value = 0; //No name or value.
#ifdef PUGOPT_NONSEG
		p->name_size = p->value_size = 0; //Lengths of zero.
#endif
		p->name_insitu = p->value_insitu = true; //Default to being in-situ of the parse string.
	}
	return p;
}


//<summary>Allocate & init an xml_node_struct structure.</summary>
//<param name="type">Desired node type.</param>
//<returns>Pointer to new xml_node_struct structure.</returns>
inline static xml_node_struct* new_node(xml_node_type type = node_element)
{
	xml_node_struct* p = (xml_node_struct*)MONITORED_MALLOC(_T("pugxml"), sizeof(xml_node_struct)); //Allocate one node.
	if(p) //If allocation succeeded.
	{
		p->name = p->value = 0; //No name or data.
#ifdef PUGOPT_NONSEG
		p->name_size = p->value_size = 0;
#endif
		p->type = type; //Set the desired type.
		p->attributes = p->children = 0; //No attributes or children.
		p->name_insitu = p->value_insitu = true; //Default to being in-situ of the parse string.
		if
		(
			type != node_document	&& //None of these will have attributes.
			type != node_pcdata		&&
			type != node_cdata		&&
			type != node_include	&&
			type != node_comment
		)
			p->attribute = (xml_attribute_struct**)MONITORED_MALLOC(_T("pugxml"), sizeof(xml_attribute_struct*)); //Allocate one attribute.
		else p->attribute = NULL;
		p->attribute_space = (p->attribute) ? 1 : 0;
		if
		(
			type == node_element || //Only these will have children.
			type == node_doctype ||
			type == node_document
		)
			p->child = (xml_node_struct**)MONITORED_MALLOC(_T("pugxml"), sizeof(xml_node_struct*)); //Allocate one child.
		else p->child = NULL;
		p->child_space = (p->child) ? 1 : 0;
#ifdef PUGOPT_NODE_FLAGS
       p->flags = 0x00;  // NF 19 Mar 2003
       p->set_visible_children( 1 );  // 17 Apr 2003
#endif
	}
	return p;
}


//<summary>Allocate & append a new xml_node_struct onto the given parent.</summary>
//<param name="parent">Pointer to parent node.</param>
//<param name="grow">Pointer space growth increment.</param>
//<param name="type">Desired node type.</param>
//<param name="xns_child">Child node to append or NULL for new node.</param>
//<since="xns_child">26 Jun 2003</since>
//<returns>Pointer to new node if successful, else NULL.</returns>
//<remarks>Child pointer space of 'node' may be reallocated.</remarks>
inline static xml_node_struct* append_node( xml_node_struct* parent, const int grow, const xml_node_type type = node_element, xml_node_struct* xns_child = NULL )
{
	if(!parent)
        return NULL; //Must have a parent.

	if(parent->children == parent->child_space) //Out of pointer space.
	{
		xml_node_struct** t = (xml_node_struct**)MONITORED_REALLOC(_T("pugxml"), parent->child,sizeof(xml_node_struct*)*(parent->child_space+grow)); //Grow pointer space.
		if(t) //Reallocation succeeded.
		{
			parent->child = t;
			parent->child_space += grow; //Update the available space.
		}
        else  // NF 26 Jun 2003
        {
            assert( t );
            return NULL;
        }
	}
	xml_node_struct* child = xns_child != NULL ? xns_child : new_node( type ); //Allocate a new child.
	child->parent = parent; //Set it's parent pointer.
	parent->child[parent->children] = child; //Set the parent's child pointer.
	parent->children++; //One more child.
	return child;
}


//<summary>Allocate & append a new attribute to the given xml_node_struct.</summary>
//<param name="node">Pointer to parent node.</param>
//<param name="grow">Pointer space growth increment.</param>
//<returns>Pointer to appended xml_attribute_struct.</returns>
//<remarks>Attribute pointer space of 'node' may be reallocated.</remarks>
inline static xml_attribute_struct* append_attribute(xml_node_struct* node,int grow)
{
	if(!node) return NULL;
	xml_attribute_struct* a = new_attribute();
	if(!a) return NULL;
	if(node->attributes == node->attribute_space) //Out of space, so grow.
	{
		xml_attribute_struct** t = (xml_attribute_struct**)MONITORED_REALLOC(_T("pugxml"), node->attribute,sizeof(xml_node_struct*)*(node->attribute_space+grow));
		if(t)
		{
			node->attribute = t;
			node->attribute_space += grow;
		}
	}
	node->attribute[node->attributes] = a;
	node->attributes++;
	return a;
}


//<summary>Non-recursively free a tree.</summary>
//<param name="root">
//	Pointer to the root of the tree. Note: 'root' must have been dynamically
//	allocated using 'malloc' or 'realloc', as 'free_node' tries to also free
//	the structure pointed to by 'root'.
//</param>
//<remarks>'root' no longer points to a valid structure.</remarks>
inline static void free_node(xml_node_struct* node)
{
	if(!node) return;

	register xml_node_struct* cursor = node;

	//Free all children of children.
	do
	{
LOC_STEP_INTO:
		for(; cursor->children>0; --cursor->children) //Free each child in turn; 'children' keeps count while we jump around.
		{
			register xml_node_struct* t = cursor->child[cursor->children-1]; //Take a pointer to the child.
			if(t && t->children) //If the child has children.
			{
				cursor = t; //Step in.
				goto LOC_STEP_INTO; //Step into this node.
			}
			else if(t)
			{
				if(t->attributes) //Child has attributes.
				{
					register unsigned int n = t->attributes; //Free each attribute.
					for(register unsigned int i=0; i<n; ++i)
					{
						if(t->attribute[i]->name && !t->attribute[i]->name_insitu)
							MONITORED_FREE(t->attribute[i]->name);
						if(t->attribute[i]->value && !t->attribute[i]->value_insitu)
							MONITORED_FREE(t->attribute[i]->value);
						MONITORED_FREE(t->attribute[i]);
					}
				}
				if(t->attribute) MONITORED_FREE(t->attribute); //Free attribute pointer space.
				if(t->child) MONITORED_FREE(t->child); //Free child pointer space.
				if(t->name && !t->name_insitu) MONITORED_FREE(t->name);
				if(t->value && !t->value_insitu) MONITORED_FREE(t->value);
				MONITORED_FREE(t); //Free the child node.
			}
		}
		cursor = cursor->parent; //Step out.
	}
	while(cursor->children); //While there are children.
	//Finally, free the root's children & the root itself.
	if(cursor->attributes)
	{
		register unsigned int n = cursor->attributes;
		for(register unsigned int i=0; i<n; ++i)
		{
			if(cursor->attribute[i]->name && !cursor->attribute[i]->name_insitu)
				MONITORED_FREE(cursor->attribute[i]->name);
			if(cursor->attribute[i]->value && !cursor->attribute[i]->value_insitu)
				MONITORED_FREE(cursor->attribute[i]->value);
			MONITORED_FREE(cursor->attribute[i]);
		}
	}
	if(cursor->attribute) MONITORED_FREE(cursor->attribute); //Free attribute pointer space.
	if(cursor->child) MONITORED_FREE(cursor->child); //Free child pointer space.
	if(cursor->name && !cursor->name_insitu) MONITORED_FREE(cursor->name); //Free name & data.
	if(cursor->value && !cursor->value_insitu) MONITORED_FREE(cursor->value);
	MONITORED_FREE(cursor); //Free the root itself.
}

//<summary>Recursively free a tree.</summary>
//<param name="root">Pointer to the root of the tree.</param>
//<remarks>Not used.</remarks>
inline static void free_node_recursive(xml_node_struct* root)
{
	if(root)
	{
		unsigned int n = root->attributes;
		register unsigned int i;
		for(i=0; i<n; i++)
		{
			if(root->attribute[i]->name && !root->attribute[i]->name_insitu)
				MONITORED_FREE(root->attribute[i]->name);
			if(root->attribute[i]->value && !root->attribute[i]->value_insitu)
				MONITORED_FREE(root->attribute[i]->value);
			MONITORED_FREE(root->attribute[i]);
		}
		MONITORED_FREE(root->attribute);
		n = root->children;
		for(i=0; i<n; i++)
			free_node_recursive(root->child[i]);
		MONITORED_FREE(root->child);
		if(root->name && !root->name_insitu) MONITORED_FREE(root->name);
		if(root->value && !root->value_insitu) MONITORED_FREE(root->value);
		MONITORED_FREE(root);
	}
}


//<summary>Parser utilities.</summary>
#define SKIPWS()			{ while(chartype_space(*s)) ++s; if(*s==0) return s; }
#define OPTSET(OPT)			( optmsk & OPT )
#define PUSHNODE(TYPE)		{ cursor = append_node(cursor,growby,TYPE); }
#define POPNODE()			{ cursor = cursor->parent; }
#define SCANFOR(X)			{ while(*s!=0 && !(X)) ++s; if(*s==0) return s; }
#define SCANWHILE(X)		{ while((X)) ++s; if(*s==0) return s; }
#ifndef PUGOPT_NONSEG
#	define ENDSEG()			{ ch = *s; *s = 0; ++s; if(*s==0) return s; }
#else
#	define ENDSEG()			{ ch = *s; ++s; if(*s==0) return s; }
#	define SETLEN()			( cursor->value_size = s - cursor->value )
#	define ENDSEGDAT()		{ ch = *s; SETLEN(); ++s; if(*s==0) return s; }
#	define ENDSEGNAM(S)		{ ch = *s; S->name_size = s - S->name; ++s; if(*s==0) return s; }
#	define ENDSEGATT(S)		{ ch = *s; S->value_size = s - S->value; ++s; if(*s==0) return s; }
#endif


//<summary>Read data from the file at 'path' into the buffer. Free with 'free'.</summary>
//<param name="path">File path.</param>
//<param name="buffer">Pointer to pointer to string to recieve buffer.</param>
//<param name="size">Pointer to count bytes read and stored in 'buffer'.</param>
//<param name="tempsize">Temporary read buffer size.</param>
//<returns>Success if file at 'path' was opened and bytes were read into memory.</returns>
//<remarks>Memory is allocated at '*buffer'. Free with 'free'.</remarks>
//inline static bool load_file(const TCHAR* path,TCHAR** buffer,unsigned int* size,unsigned int tempsize = 4096)
//{
//	if(!path || !buffer || !size) return false;
//	*size = 0;
//	*buffer = 0;
//	HANDLE file_handle = CreateFile(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
//	if(file_handle == INVALID_HANDLE_VALUE) return false;
//	TCHAR* temp = (TCHAR*) malloc(sizeof(TCHAR)*tempsize);
//	if(!temp) return false;
//	unsigned int read_bytes = 0;
//	ZeroMemory(temp,sizeof(TCHAR)*tempsize);
//	while(ReadFile(file_handle,(void*)temp,tempsize-1,&read_bytes,0) && read_bytes && strcatgrow(buffer,temp))
//	{
//		*size += read_bytes;
//		ZeroMemory(temp,sizeof(TCHAR)*tempsize);
//	}
//	CloseHandle(file_handle);
//	free(temp);
//	return (*size) ? true : false;
//}


//<summary>A void pointer array. Used by various xml_node::find* functions.</summary>
class pointer_array
{
//Internal Data Members
protected:
	unsigned int	_size; //Count items.
	unsigned int	_room; //Available space.
	void**			_data; //The list.
	unsigned int	_grow; //Grow by increment.
public:
	//<summary>Default constructor.</summary>
	//<param name="grow">Array growth increment.</param>
	pointer_array(unsigned int grow = 4):
		_size(0),
			_room(0),
			_data(NULL),
			_grow(grow)
		{
			_data = (void**)MONITORED_MALLOC(_T("pugxml"), sizeof(void*)*_grow);
			_room = (_data) ? _grow : 0;
		}
		~pointer_array(){ if(_data) MONITORED_FREE(_data); }
public:
	bool empty() const { return (_size == 0); } //True if there is no data in the array.
	void remove_all(){ _size = 0; } //Remove all data elements from the array.
	void clear() //Free any allocated memory.
	{
		if(_data)
		{
			_data = (void**)MONITORED_REALLOC(_T("pugxml"), _data,sizeof(void*)*_grow); //Reallocate to first growth increment.
			_room = _grow; //Mark it as such.
			_size = 0; //Mark array as empty.
		}
	}
	virtual void*& operator[](unsigned int i) //Access element at subscript, or dummy value if overflow.
	{
		static void* dummy = 0;
		if(i < _size) return _data[i]; else return dummy;
	}
	unsigned int size(){ return _size; } //Count data elements in the array.
	virtual void* at(unsigned int i){ if(i < _size) return _data[i]; else return NULL; } //Access element at subscript, or NULL if overflow.
	int push_back(void* element) //Append a new element to the array.
	{
		if(_data) //Fail if no array.
		{
			if(_size < _room) //There is enough allocated space.
			{
				_data[_size] = element; //Set it.
				_size++; //Increment our count of elements.
				return _size-1; //Return the element's subscript.
			}
			else //Not enough room.
			{
				void** temp = (void**)MONITORED_REALLOC(_T("pugxml"), _data,sizeof(void*)*(_size+_grow)); //Grow the array.
				if(temp) //Reallocation succeeded.
				{
					_room += _grow; //Increment available space.
					_data = temp; //Assign reallocated value to array pointer.
					_data[_size] = element; //Set the element to be added.
					_size++; //Increment our count of elements.
					return _size-1; //Return the element's subscript.
				}
			}
		}
		return -1; //Something failed, so return a bad subscript.
	}
};


//<summary>A simple indentation stack.</summary>
//<remarks>Used by xml_node::outer_xml function.</remarks>
class indent_stack
{
//Internal Data Members
protected:
	TCHAR	_inch; //The indent character.
	TCHAR*	_stac; //The aggregate indent string (stack).
	int		_size; //Current depth (avoids using '_tcslen' on push/pop).
//Construction/Destruction
public:
	//<summary>Default constructor.</summary>
	//<param name="c">Indent character.</param>
	indent_stack(TCHAR c = _T('\t')):
		_inch(c),
			_stac(0) ,
			_size(0)
		{
			_stac = (TCHAR*)MONITORED_MALLOC(_T("pugxml"), sizeof(TCHAR)); //Allocate.
			*_stac = 0; //Zero-terminate.
		}
		//Destructor.
		virtual ~indent_stack(){ if(_stac) MONITORED_FREE(_stac); }
//Stack Operators
public:
	//<summary>Grow indent string by one indent character.</summary>
	//<remarks>Reallocates the indent string.</remarks>
	void push()
	{
		if(_inch && _stac)
		{
			_size++;
			_stac = (TCHAR*)MONITORED_REALLOC(_T("pugxml"), _stac,sizeof(TCHAR)*(_size+1));
			_stac[_size-1] = _inch;
			_stac[_size] = 0;
		}
	}
	//<summary>Shrink the indent string by one indent character.</summary>
	void pop()
	{
		if(_inch && _stac && _size > 0)
		{
			_size--;
			_stac = (TCHAR*)MONITORED_REALLOC(_T("pugxml"), _stac,sizeof(TCHAR)*(_size+1));
			_stac[_size] = 0;
		}
	}
	//<summary>Accesses the indent depth.</summary>
	//<returns>The current indent string, or "" if empty.</returns>
	const TCHAR* depth() const { return (_inch && _stac) ? _stac : _T(""); }
};


//<summary>
//	Stream output. Recursively writes the given xml_node_struct structure to
//	the given stream. NOTE: Use this recursive implementation for debug purposes
//	only, since a large tree may cause a stack overflow.
//</summary>
//<param name="os">Reference to output stream.</param>
//<param name="indent">Reference to indentation stack.</param>
//<param name="node">Pointer to the node.</param>
//<param name="breaks">Use linebreaks?</param>
//<returns>
//	String data is written to stream. Indent stack may be altered.
//	If you want to make this prettier, and to avoid propagating whitespace,
//	you will have to trim excess whitespace from the PCDATA sections.
//</returns>
inline static void outer_xml(std::basic_ostream<TCHAR,std::char_traits<TCHAR> > & os,indent_stack& indent,xml_node_struct* node,bool breaks = true)
{
	if(node && os.good()) //There is a node and ostream is OK.
	{
		register unsigned int n, i;
		os << indent.depth();
		switch(node->type)
		{
		case node_dtd_attlist:
			if(node->name)
			{
#ifdef PUGOPT_NONSEG
				os << _T("<!ATTLIST ");
				os.write( node->name, node->name_size );
#else
				os << _T("<!ATTLIST ") << node->name;
#endif
				if(node->value)
#ifdef PUGOPT_NONSEG
				{
					os << _T(" ");
					os.write( node->value, node->value_size );
				}
#else
					os << _T(" ") << node->value;
#endif

				os << _T(">");
			}
			break;
		case node_dtd_element:
			if(node->name)
			{
#ifdef PUGOPT_NONSEG
				os << _T("<!ELEMENT ");
				os.write( node->name, node->name_size );
				if(node->value)
				{
					os << _T(" ");
					os.write( node->value, node->value_size );
				}
#else
				os << _T("<!ELEMENT ") << node->name;
				if(node->value) os << _T(" ") << node->value;
#endif
				os << _T(">");
			}
			break;
		case node_dtd_entity:
			if(node->name)
			{
#ifdef PUGOPT_NONSEG
				os << _T("<!ENTITY ");
				os.write( node->name, node->name_size );
				if(node->value)
				{
					os << _T(" ");
					os.write( node->value, node->value_size );
				}
#else
				os << _T("<!ENTITY ") << node->name;
				if(node->value) os << _T(" ") << node->value;
#endif
				os << _T(">");
			}
			break;
		case node_dtd_notation:
			if(node->name)
			{
#ifdef PUGOPT_NONSEG
				os << _T("<!NOTATION ");
				os.write( node->name, node->name_size );
				if(node->value)
				{
					os << _T(" ");
					os.write( node->value, node->value_size );
				}
#else
				os << _T("<!NOTATION ") << node->name;
				if(node->value) os << _T(" ") << node->value;
#endif
				os << _T(">");
			}
			break;
		case node_doctype:
			os << _T("<!DOCTYPE");
			n = node->attributes;
			for(i=0; i<n; ++i)
			{
				os << _T(" ");
				if(node->attribute[i]->name)
#ifdef PUGOPT_NONSEG
					os.write( node->attribute[i]->name, node->attribute[i]->name_size );
#else
					os << node->attribute[i]->name;
#endif
				else if(node->attribute[i]->value)
#ifdef PUGOPT_NONSEG
				{
					os << _T("\"");
					os.write( node->attribute[i]->value, node->attribute[i]->value_size );
					os << _T("\"");
				}
#else
					os << _T("\"") << node->attribute[i]->value << _T("\"");
#endif
			}
			if(node->children)
			{
				if(breaks) os << std::endl;
				else os << _T(" ");
				os << _T("[");
				if(breaks) os << std::endl;
				else os << _T(" ");
				n = node->children;
				indent.push(); //Push the indent stack.
				for(i=0; i<n; ++i)
				{
					if
					(
						node->child[i] && //There is a child at i.
						(
							node->child[i]->type == node_dtd_attlist	|| //Skip all other types.
							node->child[i]->type == node_dtd_element	||
							node->child[i]->type == node_dtd_entity		||
							node->child[i]->type == node_dtd_notation
						)
					)
						outer_xml(os,indent,node->child[i],breaks);
				}
				indent.pop(); //Pop the indent stack.
				os << _T("]");
			}
			else if(node->value)
#ifdef PUGOPT_NONSEG
			{
				os << _T(" [");
				os.write(node->value,node->value_size);
				os << _T("]");
			}
#else
				os << _T(" [") << node->value << _T("]");
#endif
			os << _T(">");
			break;
		case node_pcdata:
#ifdef PUGOPT_NONSEG
			if(node->value) os.write(node->value,node->value_size);
#else
			if(node->value) os << node->value;
#endif
			break;
		case node_cdata:
#ifdef PUGOPT_NONSEG
			if(node->value)
			{
				os << _T("<![CDATA[");
				os.write(node->value,node->value_size);
				os << _T("]]>");
			}
#else
			if(node->value) os << _T("<![CDATA[") << node->value << _T("]]>");
#endif
			break;
		case node_include:
#ifdef PUGOPT_NONSEG
			if(node->value)
			{
				os << _T("<![INCLUDE[");
				os.write(node->value, node->value_size);
				os << _T("]]>");
			}
#else
			if(node->value) os << _T("<![INCLUDE[") << node->value << _T("]]>");
#endif
			break;
		case node_comment:
#ifdef PUGOPT_NONSEG
			if(node->value)
			{
				os << _T("<!--");
				os.write(node->value, node->value_size);
				os << _T("-->");
			}
#else
			if(node->value) os << _T("<!--") << node->value << _T("-->");
#endif
			break;
		case node_element:
		case node_pi:
			os << _T("<");
			if(node->type==node_pi) os << _T("?");
			if(node->name)
#ifdef PUGOPT_NONSEG
				os.write(node->name,node->name_size);
#else
				os << node->name;
#endif
			else os << _T("anonymous");
			n = node->attributes;
			for(i=0; i<n; ++i)
			{
				if(node->attribute[i] && node->attribute[i]->name)
				{
#ifdef PUGOPT_NONSEG
					os << _T(" ");
					os.write(node->attribute[i]->name,node->attribute[i]->name_size);
					if(node->attribute[i]->value)
					{
						os << _T("=\"");
						os.write(node->attribute[i]->value,node->attribute[i]->value_size);
						os << _T("\"");
					}
#else
					os << _T(" ") << node->attribute[i]->name;
					if(node->attribute[i]->value) os << _T("=\"") << node->attribute[i]->value << _T("\"");
#endif
				}
			}
			n = node->children;
			if(n && node->type == node_element)
			{
				os << _T(">");
				if(n == 1 && node->child[0]->type == node_pcdata)
				{
					if(node->child[0] && node->child[0]->value)
#ifdef PUGOPT_NONSEG
						os.write(node->child[0]->value,node->child[0]->value_size);
#else
						os << node->child[0]->value;
#endif
				}
				else
				{
					if(breaks) os << std::endl;
					indent.push();
					for(i=0; i<n; ++i) pug::outer_xml(os,indent,node->child[i],breaks);
					indent.pop();
					os << indent.depth();
				}
				os << _T("</");
#ifdef PUGOPT_NONSEG
				if(node->name)
					os.write(node->name, node->name_size);
#else
				if(node->name) os << node->name;
#endif
				os << _T(">");
			}
			else
			{
				if(node->type==node_pi) os << _T("?>");
				else os << _T("/>");
			}
			break;
		default: break;
		}
		if(breaks) os << std::endl;
		os.flush();
	}
}


//<summary>Abstract iterator class for interating over a node's members.</summary>
//<remarks>Used as base class for 'xml_node_iterator' and 'xml_attribute_iterator'.</remarks>

template <class _Ty,class _Diff,class _Pointer,class _Reference>
class xml_iterator : public std::iterator<std::random_access_iterator_tag,_Ty,_Diff,_Pointer,_Reference>
{
protected:
	xml_node_struct* _vref; //A pointer to the node over which to iterate.
	int _sscr; //Current subscript of element.
public:
	xml_iterator() : _vref(0), _sscr(-1) {} //Default constructor.
	xml_iterator(xml_node_struct* vref,int sscr = 0) : _vref(vref), _sscr(sscr){ } //Initializing constructor.
	xml_iterator(const xml_iterator& r) : _vref(r._vref), _sscr(r._sscr){ } //Copy constructor.
	virtual ~xml_iterator(){} //Destructor.
public:
	virtual bool good() = 0; //Internal validity of '_vref'.
	virtual bool oob() = 0; //Out of bounds check for '_sscr' with respect to '_vref'. Returns true if '_sscr' is O.O.B.
public:
	virtual int subscript(){ return _sscr; } //Get subscript value;
	virtual void subscript(int new_subscript){ _sscr = new_subscript; } //Set subscript value;
public:
	virtual xml_iterator& operator=(const xml_iterator& rhs){ _vref = rhs._vref; _sscr = rhs._sscr; return *this; } //Assignment.
	virtual bool operator==(const xml_iterator& rhs){ return (_sscr == rhs._sscr); } //True if this is equal to RHS.
	virtual bool operator!=(const xml_iterator& rhs){ return (_sscr != rhs._sscr); } //True if this is not equal to RHS.
	virtual bool operator<(const xml_iterator& rhs){ return (_sscr < rhs._sscr); } //True if this subscript is less than RHS.
	virtual bool operator>(const xml_iterator& rhs){ return (_sscr > rhs._sscr); } //True if this subscript is greater than RHS.
	virtual bool operator<=(const xml_iterator& rhs){ return (_sscr <= rhs._sscr); } //True if this subscript is less than or equal to RHS.
	virtual bool operator>=(const xml_iterator& rhs){ return (_sscr >= rhs._sscr); } //True if this subscript is greater than or equal to RHS.
	virtual xml_iterator& operator++(){ _sscr++; return *this; } //Increment the iterator (subscript).
	virtual xml_iterator& operator--(){ _sscr--; return *this; } //Decrement the iterator (subscript).
	virtual _Ty& operator*() = 0; //Dereference operator.
	virtual _Ty* operator->() = 0;
};

class xml_node; //Forward decl.


//<summary>Abstract tree walker class for xml_node::traverse().</summary>
class xml_tree_walker
{
protected:
	int _deep; //Current node depth.
public:
	xml_tree_walker() : _deep(0) {} //Default constructor.
	virtual ~xml_tree_walker(){} //Destructor.
public:
	virtual void push(){ ++_deep; } //Increment node depth.
	virtual void pop(){ --_deep; } //Decrement node depth.
	virtual int depth() const { return (_deep > 0) ? _deep : 0; } //Access node depth.
public:
	//<summary>Callback when traverse on a given root node begins.</summary>
	//<returns>Returning false will abort the traversal.</returns>
	//<remarks>Override this to implement your own custom behavior.</remarks>
	virtual bool begin(xml_node&){ return true; }

	//<summary>Callback for each node that is hit on traverse.</summary>
	//<param name="bDescend">return true to descend this branch,
    // else false to continue at next child.</param>
    //<since>bDescend NF 18 Sep 2003</since>
	//<returns>Returning false will abort the traversal.</returns>
	virtual bool for_each(xml_node&, bool& bDescend ) = 0;

	//<summary>Callback when traverse on a given root node ends.</summary>
	//<returns>Returning false will abort the traversal.</returns>
	//<remarks>Override this to implement your own custom behavior.</remarks>
	virtual bool end(xml_node&){ return true; }
};


//<summary>Provides a light-weight wrapper for manipulating xml_attribute_struct structures.</summary>
//<remarks>
//	Note: xml_attribute does not create any memory for the attribute it wraps;
//	it only wraps a pointer to an existing xml_attribute_struct.
//</remarks>
class xml_attribute
{
//Internal Data Members
protected:
	xml_attribute_struct* _attr; //The internal attribute pointer.
    bool _delete_attr;
//Construction/Destruction
public:
	xml_attribute() : _attr(NULL), _delete_attr(false) {} //Default constructor.
	xml_attribute(xml_attribute_struct* attr) : _attr(attr), _delete_attr(false) {} //Initializing constructor.
	xml_attribute(xml_attribute_struct* attr, const bool delete_attr) : _attr(attr), _delete_attr(delete_attr) {} //Initializing constructor. NF 3 Mar 2003
	xml_attribute(const xml_attribute& r) : _attr(r._attr),  _delete_attr(r._delete_attr) {} //Copy constructor.
	virtual ~xml_attribute(){ if  (_delete_attr) delete _attr; } //Destructor.
//Operators
public:
	void attach(xml_attribute_struct* v){ _attr = v; }
	xml_attribute& operator=(const xml_attribute& r){ _attr = r._attr; return *this; } //Assign internal pointer.
	bool operator==(const xml_attribute& r){ return (_attr == r._attr); } //Compare internal pointer.
	bool operator!=(const xml_attribute& r){ return (_attr != r._attr); }
	operator xml_attribute_struct*(){ return _attr; }
	//<summary>Cast attribute value as CEasyString. If not found, return empty.</summary>
	//<returns>The CEasyString attribute value, or empty.</returns>
	//<remarks>Note: Modifying this will not change the value, e.g. read only.</remarks>
	operator CEasyString()
	{
		CEasyString temp;
		if(has_value())
		{
#ifdef PUGOPT_NONSEG
			// temp.append(_attr->value,_attr->value_size);
			temp.SetString(_attr->value,_attr->value_size);  // NF 2 Mar 2003
#else
			temp = _attr->value;
#endif
		}
		XMLDecodeString(temp);
		return temp;
	}
	//<summary>Cast attribute value as integral character string. If not found, return NULL.</summary>
	//<returns>Integral character string attribute value, or NULL.</returns>
	//<remarks>Warning: Modifying this may corrupt portions of the document tree.</remarks>
	operator const TCHAR*()
	{
		if(!has_value()) return NULL;
		return _attr->value;
	}
	//<summary>Cast attribute value as int. If not found, return 0.</summary>
	//<returns>Attribute value as int, or 0.</returns>
	//<remarks>Note: Modifying this will not change the value, e.g. read only.
        //         The attribute value can be Dec|Octal|Hex. eg. 123 | 074 | 0xEF
        //</remarks>
        //<since>3 Mar 2003 NF</since>
	operator int()
	{
		if(!has_value()) return 0;
#ifdef PUGOPT_NONSEG
		TCHAR temp[PUGDEF_ATTR_VALU_SIZE];
		unsigned int valulen = sizeof(temp)-1;
		const unsigned int maxlen = valulen ? Min(valulen,_attr->value_size) : _attr->value_size;
		_tcsncpy_s(temp,PUGDEF_ATTR_VALU_SIZE,_attr->value,maxlen);
		temp[maxlen] = 0;
                return (int)_tcstol( temp, NULL, 0 );    // rem: base = 0 so value can be Dec|Octal|Hex
#else
                return (int)_tcstol( _attr->value, NULL, 0 ); // rem: base = 0 so value can be Dec|Octal|Hex
#endif
	}

	operator unsigned int()
	{
		if(!has_value()) return 0;
#ifdef PUGOPT_NONSEG
		TCHAR temp[PUGDEF_ATTR_VALU_SIZE];
		unsigned int valulen = sizeof(temp)-1;
		const unsigned int maxlen = valulen ? Min(valulen,_attr->value_size) : _attr->value_size;
		_tcsncpy_s(temp,PUGDEF_ATTR_VALU_SIZE,_attr->value,maxlen);
		temp[maxlen] = 0;
		return (unsigned int)_tcstoul( temp, NULL, 0 );    // rem: base = 0 so value can be Dec|Octal|Hex
#else
		return (unsigned int)_tcstoul( _attr->value, NULL, 0 ); // rem: base = 0 so value can be Dec|Octal|Hex
#endif
	}

	operator short()
	{
		if(!has_value()) return 0;
#ifdef PUGOPT_NONSEG
		TCHAR temp[PUGDEF_ATTR_VALU_SIZE];
		unsigned int valulen = sizeof(temp)-1;
		const unsigned int maxlen = valulen ? Min(valulen,_attr->value_size) : _attr->value_size;
		_tcsncpy_s(temp,PUGDEF_ATTR_VALU_SIZE,_attr->value,maxlen);
		temp[maxlen] = 0;
		return (int)_tcstol( temp, NULL, 0 );    // rem: base = 0 so value can be Dec|Octal|Hex
#else
		return (int)_tcstol( _attr->value, NULL, 0 ); // rem: base = 0 so value can be Dec|Octal|Hex
#endif
	}

	operator unsigned short()
	{
		if(!has_value()) return 0;
#ifdef PUGOPT_NONSEG
		TCHAR temp[PUGDEF_ATTR_VALU_SIZE];
		unsigned int valulen = sizeof(temp)-1;
		const unsigned int maxlen = valulen ? Min(valulen,_attr->value_size) : _attr->value_size;
		_tcsncpy_s(temp,PUGDEF_ATTR_VALU_SIZE,_attr->value,maxlen);
		temp[maxlen] = 0;
		return (unsigned int)_tcstoul( temp, NULL, 0 );    // rem: base = 0 so value can be Dec|Octal|Hex
#else
		return (unsigned int)_tcstoul( _attr->value, NULL, 0 ); // rem: base = 0 so value can be Dec|Octal|Hex
#endif
	}

	operator char()
	{
		if(!has_value()) return 0;
#ifdef PUGOPT_NONSEG
		TCHAR temp[PUGDEF_ATTR_VALU_SIZE];
		unsigned int valulen = sizeof(temp)-1;
		const unsigned int maxlen = valulen ? Min(valulen,_attr->value_size) : _attr->value_size;
		_tcsncpy_s(temp,PUGDEF_ATTR_VALU_SIZE,_attr->value,maxlen);
		temp[maxlen] = 0;
		return (char)_tcstol( temp, NULL, 0 );    // rem: base = 0 so value can be Dec|Octal|Hex
#else
		return (char)_tcstol( _attr->value, NULL, 0 ); // rem: base = 0 so value can be Dec|Octal|Hex
#endif
	}

	operator unsigned char()
	{
		if(!has_value()) return 0;
#ifdef PUGOPT_NONSEG
		TCHAR temp[PUGDEF_ATTR_VALU_SIZE];
		unsigned int valulen = sizeof(temp)-1;
		const unsigned int maxlen = valulen ? Min(valulen,_attr->value_size) : _attr->value_size;
		_tcsncpy_s(temp,PUGDEF_ATTR_VALU_SIZE,_attr->value,maxlen);
		temp[maxlen] = 0;
		return (unsigned char)_tcstoul( temp, NULL, 0 );    // rem: base = 0 so value can be Dec|Octal|Hex
#else
		return (unsigned char)_tcstoul( _attr->value, NULL, 0 ); // rem: base = 0 so value can be Dec|Octal|Hex
#endif
	}

	//<summary>Cast attribute value as int. If not found, return 0.</summary>
	//<returns>Attribute value as int, or 0.</returns>
	//<remarks>Note: Modifying this will not change the value, e.g. read only.</remarks>
//	operator int()
//	{
//		if(!has_value()) return 0;
//#ifdef PUGOPT_NONSEG
//		TCHAR temp[PUGDEF_ATTR_VALU_SIZE];
//		unsigned int valulen = sizeof(temp)-1;
//		const unsigned int maxlen = valulen ? Min(valulen,_attr->value_size) : _attr->value_size;
//		_tcsncpy_s(temp,PUGDEF_ATTR_VALU_SIZE,_attr->value,maxlen);
//		temp[maxlen] = 0;
//		return _tcstol(temp,NULL,10);
//#else
//		return _tcstol(_attr->value,NULL,10);
//#endif
//	}
//
//	operator unsigned int()
//	{
//		if(!has_value()) return 0;
//#ifdef PUGOPT_NONSEG
//		TCHAR temp[PUGDEF_ATTR_VALU_SIZE];
//		unsigned int valulen = sizeof(temp)-1;
//		const unsigned int maxlen = valulen ? Min(valulen,_attr->value_size) : _attr->value_size;
//		_tcsncpy_s(temp,PUGDEF_ATTR_VALU_SIZE,_attr->value,maxlen);
//		temp[maxlen] = 0;
//		return _tcstoul(temp,NULL,10);
//#else
//		return _tcstoul(_attr->value,NULL,10);
//#endif
//	}

	operator __int64()
	{
		if(!has_value()) return 0;
#ifdef PUGOPT_NONSEG
		TCHAR temp[PUGDEF_ATTR_VALU_SIZE];
		unsigned int valulen = sizeof(temp)-1;
		const unsigned int maxlen = valulen ? Min(valulen,_attr->value_size) : _attr->value_size;
		_tcsncpy_s(temp,PUGDEF_ATTR_VALU_SIZE,_attr->value,maxlen);
		temp[maxlen] = 0;
		return _tcstoi64(temp,NULL,10);
#else
		return _tcstoi64(_attr->value,NULL,10);
#endif
	}

	operator unsigned __int64()
	{
		if(!has_value()) return 0;
#ifdef PUGOPT_NONSEG
		TCHAR temp[PUGDEF_ATTR_VALU_SIZE];
		unsigned int valulen = sizeof(temp)-1;
		const unsigned int maxlen = valulen ? Min(valulen,_attr->value_size) : _attr->value_size;
		_tcsncpy_s(temp,PUGDEF_ATTR_VALU_SIZE,_attr->value,maxlen);
		temp[maxlen] = 0;
		return _tcstoui64(temp,NULL,10);
#else
		return _tcstoui64(_attr->value,NULL,10);
#endif
	}

	//<summary>Cast attribute value as double. If not found, return 0.0.</summary>
	//<returns>Attribute value as double, or 0.0.</returns>
	//<remarks>Note: Modifying this will not change the value, e.g. read only.</remarks>
	operator double()
	{
		if(!has_value()) return 0.0;
#ifdef PUGOPT_NONSEG
		TCHAR temp[PUGDEF_ATTR_VALU_SIZE];
		unsigned int valulen = sizeof(temp)-1;
		const unsigned int maxlen = valulen ? Min(valulen,_attr->value_size) : _attr->value_size;
		_tcsncpy_s(temp,PUGDEF_ATTR_VALU_SIZE,_attr->value,maxlen);
		temp[maxlen] = 0;
		return _tcstod(temp,0);
#else
		return _tcstod(_attr->value,0);
#endif
	}
	operator float()
	{
		if(!has_value()) return 0.0;
#ifdef PUGOPT_NONSEG
		TCHAR temp[PUGDEF_ATTR_VALU_SIZE];
		unsigned int valulen = sizeof(temp)-1;
		const unsigned int maxlen = valulen ? Min(valulen,_attr->value_size) : _attr->value_size;
		_tcsncpy_s(temp,PUGDEF_ATTR_VALU_SIZE,_attr->value,maxlen);
		temp[maxlen] = 0;
		return (float)_tcstod(temp,0);
#else
		return (float)_tcstod(_attr->value,0);
#endif
	}
	//<summary>Cast attribute value as bool. If not found, return false.</summary>
	//<returns>Attribute value as bool, or false.</returns>
	//<remarks>Note: Modifying this will not change the value, e.g. read only.</remarks>
	operator bool()
	{
		if(!has_value()) return false;
		if(*(_attr->value))
		{
			return //Only look at first char:
			(
				*(_attr->value) == _T('1') || //1*
				*(_attr->value) == _T('t') || //t* (true)
				*(_attr->value) == _T('T') || //T* (True|TRUE)
				*(_attr->value) == _T('y') || //y* (yes)
				*(_attr->value) == _T('Y') //Y* (Yes|YES)
			)
				? true : false; //Return true if matches above, else false.
		}
		return false;
	}
	//<summary>Set attribute to CEasyString.</summary>
	//<param name="rhs">Value CEasyString to set.</param>
	//<returns>Reference to xml_attribute.</returns>
	//Need Recovery xml_attribute& operator=(const CEasyString& rhs){ value(rhs.GetBuffer()); return *this; }
	//<summary>Set attribute to string.</summary>
	//<param name="rhs">Value string to set.</param>
	//<returns>Reference to xml_attribute.</returns>
	xml_attribute& operator=(const TCHAR* rhs){ if(rhs) value(rhs); return *this; }
	//<summary>Set attribute to int.</summary>
	//<param name="rhs">Value int to set.</param>
	//<returns>Reference to xml_attribute.</returns>
    //<since>5 Mar 2003 NF</since>
	xml_attribute& operator=(const int rhs)
	{
		TCHAR temp[32] = {0};
        _itot_s( rhs, temp,32, 10 );
		value(temp);
		return *this;
	}
	//<summary>Set attribute to int.</summary>
	//<param name="rhs">Value int to set.</param>
	//<returns>Reference to xml_attribute.</returns>
	//xml_attribute& operator=(const int rhs)
	//{
	//	TCHAR temp[32] = {0};
	//	// _stprintf(temp,_T("%ld"),rhs);
 //       _ltot_s( rhs, temp,32, 10 );    // NF changed from _stprintf 5 Mar 2003
	//	value(temp);
	//	return *this;
	//}
	//<summary>Set attribute to double.</summary>
	//<param name="rhs">Value double to set.</param>
	//<returns>Reference to xml_attribute.</returns>
	xml_attribute& operator=(const double rhs)
	{
		TCHAR temp[32] = {0};
		_stprintf_s(temp,32,_T("%lf"),rhs);
		value(temp);
		return *this;
	}
	//<summary>Set attribute to bool.</summary>
	//<param name="rhs">Value bool to set.</param>
	//<returns>Reference to xml_attribute.</returns>
	xml_attribute& operator=(const bool rhs)
	{
		value(rhs?_T("true"):_T("false"));
		return *this;
	}
	//<summary>Right-shift attribute value to CEasyString.</summary>
	//<param name="rhs">Reference to CEasyString to set.</param>
	//<returns>Reference to xml_attribute.</returns>
	xml_attribute& operator>>(CEasyString& rhs)
	{
#ifdef PUGOPT_NONSEG
		// rhs.clear();  // NF 2 Mar 2003
		// rhs.append(_attr->value,_attr->value_size);
		rhs.SetString(_attr->value,_attr->value_size);
#else
		rhs = value();
#endif
		return *this;
	}
	//<summary>Right-shift attribute value to int.</summary>
	//<param name="rhs">Reference to int to set.</param>
	//<returns>Reference to xml_attribute.</returns>
	xml_attribute& operator>>(int& rhs){ rhs = (int)*this; return *this; }
	//<summary>Right-shift attribute value to double.</summary>
	//<param name="rhs">Reference to double to set.</param>
	//<returns>Reference to xml_attribute.</returns>
	xml_attribute& operator>>(double& rhs){ rhs = (double)*this; return *this; }
	//<summary>Right-shift attribute value to bool.</summary>
	//<param name="rhs">Reference to bool to set.</param>
	//<returns>Reference to xml_attribute.</returns>
	xml_attribute& operator>>(bool& rhs){ rhs = (bool)*this; return *this; }
	//<summary>Left-shift attribute value to int.</summary>
	//<param name="lhs">Reference to int to set.</param>
	//<param name="rhs">Reference to xml_attribute to read.</param>
	//<returns>Reference to int.</returns>
	friend int& operator<<(int& lhs,xml_attribute& rhs){ lhs = (int)rhs; return lhs; }
	//<summary>Left-shift attribute value to double.</summary>
	//<param name="lhs">Reference to double to set.</param>
	//<param name="rhs">Reference to xml_attribute to read.</param>
	//<returns>Reference to double.</returns>
	friend double& operator<<(double& lhs,xml_attribute& rhs){ lhs = (double)rhs; return lhs; }
	//<summary>Left-shift attribute value to bool.</summary>
	//<param name="lhs">Reference to bool to set.</param>
	//<param name="rhs">Reference to xml_attribute to read.</param>
	//<returns>Reference to bool.</returns>
	friend bool& operator<<(bool& lhs,xml_attribute& rhs){ lhs = (bool)rhs; return lhs; }
	//<summary>Left-shift int to attribute value.</summary>
	//<param name="lhs">Reference to xml_attribute to set.</param>
	//<param name="rhs">Reference to int to read.</param>
	//<returns>Reference to xml_attribute.</returns>
	friend xml_attribute& operator<<(xml_attribute& lhs,const int rhs){ lhs = rhs; return lhs; }
	//<summary>Left-shift double to attribute value.</summary>
	//<param name="lhs">Reference to xml_attribute to set.</param>
	//<param name="rhs">Reference to double to read.</param>
	//<returns>Reference to xml_attribute.</returns>
	friend xml_attribute& operator<<(xml_attribute& lhs,const double& rhs){ lhs = rhs; return lhs; }
	//<summary>Left-shift bool to attribute value.</summary>
	//<param name="lhs">Reference to xml_attribute to set.</param>
	//<param name="rhs">Reference to bool to read.</param>
	//<returns>Reference to xml_attribute.</returns>
	friend xml_attribute& operator<<(xml_attribute& lhs,const bool& rhs){ lhs = rhs; return lhs; }
public:
	bool empty() const { return (_attr == NULL); } //True if the internal xml_attribute_struct pointer is NULL.
	bool has_name() const { return (!empty() && _attr->name); } //True if the attribute has a name.
	bool has_value() const { return (!empty() && _attr->value); } //True if the attribute has a value.
#ifdef PUGOPT_NONSEG
	bool has_name(const TCHAR* name) const { return (name && !empty() && has_name() && _tcsncmp(_attr->name,name,_attr->name_size)==0); } //Is named 'name'.
	bool has_value(const TCHAR* value) const { return (value && !empty() && has_value() && _tcsncmp(_attr->value,value,_attr->value_size)==0); } //Has value 'value'.
#else
	bool has_name(const TCHAR* name) const { return (name && !empty() && has_name() && _tcscmp(_attr->name,name)==0); } //Is named 'name'.
	bool has_value(const TCHAR* value) const { return (value && !empty() && has_value() && _tcscmp(_attr->value,value)==0); } //Has value 'value'.
#endif
public:
	const TCHAR* name() const { return (!empty() && _attr->name) ? _attr->name : _T(""); } //Access the attribute name.
    //Access the attribute name.
    //<since>2 Mar 2003 NF</since>
	CEasyString getname()
    {
		CEasyString temp;
		if( has_name() )
		{
#ifdef PUGOPT_NONSEG
			temp.SetString(_attr->name,_attr->name_size);
#else
			temp = _attr->name;
#endif
		}
		return temp;
	}

#ifdef PUGOPT_NONSEG
	unsigned int name_size(){ return (!empty()) ? _attr->name_size : 0; } //Access the attribute name length (for PUGOPT_NONSEG).
#endif
	bool name(TCHAR* new_name) //Set the attribute name.
	{
		if(!empty() && new_name)
#ifdef PUGOPT_NONSEG
			return strcpyinsitu(&_attr->name,new_name,&_attr->name_insitu,_attr->name_size);
#else
			return strcpyinsitu(&_attr->name,new_name,&_attr->name_insitu);
#endif
		return false;
	}
	const TCHAR* value(){ return (!empty()) ? _attr->value : _T(""); } //Access the attribute value.
	CEasyString getvalue()
	{
		return *this;
	} // Access the attribute value. uses operator CEasyString(). NF 2 Mar 2003
#ifdef PUGOPT_NONSEG
	unsigned int value_size(){ return (!empty()) ? _attr->value_size : 0; } //Access the attribute name length (for PUGOPT_NONSEG).
#endif
	bool value(const TCHAR* new_value) //Set the attribute value.
	{
		if(!empty() && new_value)
#ifdef PUGOPT_NONSEG
			return strcpyinsitu(&_attr->value,new_value,&_attr->value_insitu,_attr->value_size);
#else
			return strcpyinsitu(&_attr->value,new_value,&_attr->value_insitu);
#endif
		return false;
	}
};


class xml_node; //Forward declaration.


//<summary>Forward wrapper for any as-yet undefined class.</summary>
//<remarks>
//	Used by xml_node_iterator, and xml_attribute_iterator to assist with
//	operator->(), and operator*() mapping to xml_node and xml_attribute
//	types.
//</remarks>
template <typename TYPE> class forward_class
{
protected:
	TYPE* _obj; //The class, internal.
public:
	forward_class() : _obj(NULL) { _obj = MONITORED_NEW(_T("pugxml"), TYPE); } //Default constructor.
	forward_class(const TYPE& r) : _obj(NULL) { _obj = MONITORED_NEW(_T("pugxml"), TYPE, r); } //Copy constructor.
	virtual ~forward_class(){ if(_obj) MONITORED_DELETE(_obj); } //Destructor.
public:
	TYPE& operator* (){ return *_obj; }	//Dereference to the class.
	TYPE* operator->(){ return _obj; }	//Class member selection.
	operator TYPE (){ return *_obj; }	//Cast as class type.
	operator TYPE&(){ return *_obj; }	//Cast as class type reference.
	operator TYPE*(){ return _obj; }	//Cast as class type pointer.
    forward_class& operator=(const forward_class& rhs){ return *this; } // 7 Oct 2003 See: http://www.codeproject.com/soap/pugxml.asp?msg=620173#xx620173xx
};


//<summary>Provides a light-weight wrapper for manipulating xml_node_struct structures.</summary>
class xml_node
{
//Internal Data Members
protected:

	xml_node_struct* _root; //Pointer to node root.
	xml_node_struct _dummy; //Utility.


//Construction/Destruction
public:

	//<summary>Default constructor.</summary>
	//<remarks>
	//	Node root points to a dummy 'xml_node_struct' structure. Test for this
	//	with 'empty'.
	//</remarks>
	xml_node(): _root(0)
	{
		memset(&_dummy,0,sizeof(xml_node_struct));
		_dummy.type = node_null;
		_dummy.parent = &_dummy;
		_root = &_dummy;
	}

	//<summary>Construct, wrapping the given 'xml_node_struct' pointer.</summary>
	//<param name="p">Pointer to node to wrap.</param>
	//<remarks>It is possible that 'p' is NULL, so test for this with 'empty'.</remarks>
	xml_node(xml_node_struct* p): _root(p) { memset(&_dummy,0,sizeof(xml_node_struct)); }

	//<summary>Copy constructor.</summary>
	//<param name="r">Reference to node.</param>
	//<remarks>
	//	Only the root pointer is assigned, so both classes now in fact point
	//	to the same structure.
	//</remarks>
	xml_node(const xml_node& r): _root(r._root)
    {
        if (r._root->type == node_null)  // fix as per: http://www.codeproject.com/soap/pugxml.asp?msg=609169#xx609169xx 7 Oct 2003
        {
            memset(&_dummy,0,sizeof(xml_node_struct));
            _dummy.type = node_null;
            _dummy.parent = &_dummy;
            _root = &_dummy;
        }
    }

	//<summary>Destructor.</summary>
	virtual ~xml_node(){}

	//<summary>Attach to the given structure.</summary>
	//<param name="p">Pointer to node structure to wrap.</param>
	//<returns>Pointer to previous node structure.</returns>
	xml_node_struct* attach(xml_node_struct* p)
	{
		xml_node_struct* prev = _root;
		_root = p;
		return prev;
	}





//Iteration
public:

	//<summary>Child node iterator.</summary>

	class xml_node_iterator : public xml_iterator<xml_node,int,xml_node*,xml_node&>
	{
	protected:
		forward_class<xml_node> _wrap; //Wrapper for xml_node.
	public:

        xml_node_iterator() : _wrap(), xml_iterator<xml_node,int,xml_node*,xml_node&>() {} //Default constructor.
        xml_node_iterator(xml_node_struct* vref,int sscr = 0) : _wrap(), xml_iterator<xml_node,int,xml_node*,xml_node&>(vref,sscr) { } //Initializing constructor.
        xml_node_iterator(const xml_node_iterator& r) : _wrap(), xml_iterator<xml_node,int,xml_node*,xml_node&>(r) { } //Copy constructor.
		virtual bool good() //Internal validity.
		{
			if
			(
				_vref != 0			&& //Pointing to some node.
				_vref->child != 0	&& //The node has an array of children.
				_vref->children > 0    //There are 1 or more children in the array.
			)
				return true;
			return false;
		}
		virtual bool oob() //Out of bounds check.
		{
			if
			(
				!good()		|| //There is no data over which to iterate.
				_sscr < 0	|| //Subscript is out of range.
				_sscr >= (int)_vref->children
			)
				return true;
			return false;
		}
		//<summary>Pointer dereference for current xml_node.<summary>
		//<returns>
		//	Reference to the internal xml_node object, which wraps the
		//	xml_node_struct corresponding to the node at the
		//	current subscript.
		//</returns>
		virtual xml_node& operator*()
		{
			if(!oob()) _wrap->attach(_vref->child[_sscr]);
			else _wrap->attach(NULL);
			return (xml_node&)_wrap;
		}
		virtual xml_node* operator->() //Member selection for current xml_node.
		{
			if(!oob()) _wrap->attach(_vref->child[_sscr]);
			else _wrap->attach(NULL);
			return (xml_node*)_wrap;
		}
	};

	//<summary>Attribute iterator.</summary>

	class xml_attribute_iterator : public xml_iterator<xml_attribute,int,xml_attribute*,xml_attribute&>
	{
	protected:
		forward_class<xml_attribute> _wrap;
	public:

        xml_attribute_iterator() : _wrap(), xml_iterator<xml_attribute,int,xml_attribute*,xml_attribute&>() {} //Default constructor.
        xml_attribute_iterator(xml_node_struct* vref,int sscr = 0) : _wrap(), xml_iterator<xml_attribute,int,xml_attribute*,xml_attribute&>(vref,sscr) { } //Initializing constructor.
        xml_attribute_iterator(const xml_attribute_iterator& r) : _wrap(), xml_iterator<xml_attribute,int,xml_attribute*,xml_attribute&>(r) { } //Copy constructor.
		virtual bool good() //Internal validity check.
		{
			if
			(
				_vref != 0				&& //Pointing to some node.
				_vref->attribute != 0	&& //The node has an array of children.
				_vref->attributes > 0	   //There are 1 or more children in the array.
			)
				return true;
			return false;
		}
		virtual bool oob() //Out of bounds check.
		{
			if
			(
				!good()		|| //There is no data over which to iterate.
				_sscr < 0	|| //Subscript is out of range.
				_sscr >= (int)_vref->attributes //For 'end'
			)
				return true;
			return false;
		}
		//<summary>Pointer dereference for current xml_attribute.</summary>
		//<returns>
		//	Reference to the internal xml_attribute object, which wraps the
		//	xml_attribute_struct corresponding to the attribute at the
		//	current subscript.
		//</returns>
		virtual xml_attribute& operator*()
		{
			if(!oob()) _wrap->attach(_vref->attribute[_sscr]);
			else _wrap->attach(NULL);
			return (xml_attribute&)_wrap;
		}
		//<summary>Member selection for current xml_attribute.</summary>
		//<returns></returns>
		virtual xml_attribute* operator->()
		{
			if(!oob()) _wrap->attach(_vref->attribute[_sscr]);
			else _wrap->attach(NULL);
			return (xml_attribute*)_wrap;
		}
	};

	//<summary>Base iterator type (for child nodes). Same as 'child_iterator'.</summary>
	typedef xml_node_iterator iterator;
	//<summary>Base iterator type (for child nodes). Same as 'iterator'.</summary>
	typedef xml_node_iterator child_iterator;
	//<summary>Base iterator type (for sibling nodes). Same as 'iterator'.</summary>
	typedef xml_node_iterator sibling_iterator;
	//<summary>Attribute iterator type.</summary>
	typedef xml_attribute_iterator attribute_iterator;

	//<summary>Access the begin iterator for this node's collection of child nodes.</summary>
	//<returns>The begin iterator for this node's collection of child nodes.</returns>
	//<remarks>Same as 'children_begin'.</remarks>
	iterator begin(){ return iterator(_root,0); }
	//<summary>Access the end iterator for this node's collection of child nodes.</summary>
	//<returns>The end iterator for this node's collection of child nodes.</returns>
	//<remarks>Same as 'children_end'.</remarks>
	iterator end(){ return iterator(_root,_root->children); }
	//<summary>Erase the given node from node's collection of child nodes.</summary>
	//<returns>The begin iterator for this node's collection of child nodes.</returns>
	//<remarks>Same as 'children_erase'.</remarks>
	iterator erase(iterator where){ remove_child((unsigned int)where.subscript()); return iterator(_root,0); }

	//<summary>Access the begin iterator for this node's collection of child nodes.</summary>
	//<returns>The begin iterator for this node's collection of child nodes.</returns>
	//<remarks>Same as 'begin'.</remarks>
	child_iterator children_begin(){ return child_iterator(_root,0); }
	//<summary>Access the end iterator for this node's collection of child nodes.</summary>
	//<returns>The end iterator for this node's collection of child nodes.</returns>
	//<remarks>Same as 'end'.</remarks>
	child_iterator children_end(){ return child_iterator(_root,_root->children); }
	//<summary>Erase the given node from node's collection of child nodes.</summary>
	//<returns>The begin iterator for this node's collection of child nodes.</returns>
	//<remarks>Same as 'erase'.</remarks>
	child_iterator children_erase(child_iterator where){ remove_child((unsigned int)where.subscript()); return child_iterator(_root,0); }

	//<summary>Access the begin iterator for this node's collection of attributes.</summary>
	//<returns>The begin iterator for this node's collection of attributes.</returns>
	attribute_iterator attributes_begin() const { return attribute_iterator(_root,0); }
	//<summary>Access the end iterator for this node's collection of attributes.</summary>
	//<returns>The end iterator for this node's collection of attributes.</returns>
	attribute_iterator attributes_end() const { return attribute_iterator(_root,_root->attributes); }
	//<summary>Erase the given attribute from node's collection of attributes.</summary>
	//<returns>The begin iterator for this node's collection of attributes.</returns>
	attribute_iterator attributes_erase(attribute_iterator where){ remove_attribute((unsigned int)where.subscript()); return attribute_iterator(_root,0); }

	//<summary>Access the begin iterator for this node's collection of siblings.</summary>
	//<returns>The begin iterator for this node's collection of siblings.</returns>
	// sibling_iterator siblings_begin(){ if(!empty()) return sibling_iterator(_root->parent,0); return sibling_iterator(); }
	sibling_iterator siblings_begin(){ if(has_siblings()) return sibling_iterator(_root->parent,0); return sibling_iterator(); }
	//<summary>Access the end iterator for this node's collection of siblings.</summary>
	//<returns>The end iterator for this node's collection of siblings.</returns>
	// sibling_iterator siblings_end(){ if(!empty()) return sibling_iterator(_root->parent,_root->parent->children); return sibling_iterator(); }
	sibling_iterator siblings_end(){ if(has_siblings()) return sibling_iterator(_root->parent,_root->parent->children); return sibling_iterator(); }
	//<summary>Erase the given sibling from node's collection of siblings.</summary>
	//<returns>The begin iterator for this node's collection of siblings.</returns>
	sibling_iterator siblings_erase(sibling_iterator where){ parent().remove_child((unsigned int)where.subscript()); return iterator(_root->parent,0); }

//Overloaded Operators
public:

	operator xml_node_struct*() const { return _root; } //Cast as xml_node_struct pointer.
	operator void*() const { return (void*)_root; } //Cast root as void*.
	xml_node& operator=(const xml_node& r){ _root = r._root; return *this; } //Assign to xml_node_struct pointer.
	bool operator==(const xml_node& r){ return (_root == r._root); } //True if this has the same internal xml_node_struct pointer value.
	xml_node operator[](unsigned int i){ return child(i); } //Access the child at subscript.

//Node Classification
public:

	bool empty() const { return (_root == 0 || _root->type == node_null); } //Node pointer is null, or type is node_null. Same as type_null.
	bool type_null() const { return empty(); } //Node pointer is null, or type is node_null. Same as empty.
	bool type_document() const { return (_root && _root == _root->parent && _root->type == node_document); } //Node is tree root.
	bool type_element() const { return (_root && _root->type == node_element); } //Node is an element.
	bool type_comment() const { return (_root && _root->type == node_comment); } //Node is a comment.
	bool type_pcdata() const { return (_root && _root->type == node_pcdata); } //Node is PCDATA.
	bool type_cdata() const { return (_root && _root->type == node_cdata); } //Node is CDATA.
	bool type_include() const { return (_root && _root->type == node_include); } //Node is INCLUDE.
	bool type_pi() const { return (_root && _root->type == node_pi); } //Node is a processing instruction.
	bool type_doctype() const { return (_root && _root->type == node_doctype); } //Node is DOCTYPE.
	bool type_dtd_item() const { return (_root && _root->type > node_doctype); } //Node is NODE_DTD_*.
	bool type_dtd_attlist() const { return (_root && _root->type == node_dtd_attlist); } //Node is node_dtd_attlist.
	bool type_dtd_element() const { return (_root && _root->type == node_dtd_element); } //Node is node_dtd_element.
	bool type_dtd_entity() const { return (_root && _root->type == node_dtd_entity); } //Node is node_dtd_entity.
	bool type_dtd_notation() const { return (_root && _root->type == node_dtd_notation); } //Node is node_dtd_notation.

//Member Inventory
public:

	bool has_value() const { return (!empty() && _root->value != 0); } //Node has data (comment, CDATA or PCDATA).
	bool has_child_nodes() const { return (!empty() && children() > 0); } //Node has 1 or more children.
	bool has_attributes() const { return (!empty() && attributes() > 0); } //Node has 1 or more attributes.
	bool has_siblings() const { return (!empty() && siblings() > 0); } //Node has one or more siblings.
	bool has_name() const { return (!empty() && _root->name != 0); } //Node has a name.
	//Need Recovery bool has_name(const CEasyString& name) const { return has_name(name.GetBuffer()); } //Node is named 'name'.
	//Need Recovery bool has_attribute(const CEasyString& name) const { return has_attribute(name.GetBuffer()); } //Node has an attribute named 'name'.
#ifdef PUGOPT_NONSEG
	bool has_name(const TCHAR* name) const { return (name && _root && _root->name && _tcsncmp(_root->name,name,_root->name_size)==0); } //Node is named 'name'.
#else
	bool has_name(const TCHAR* name) const { return (name && _root && _root->name && strcmpwild(name,_root->name)==0); } //Node is named 'name'.
#endif
	bool has_attribute(const TCHAR* name) const { return (mapto_attribute_idx(name) > -1); } //Node has an attribute named name.

//Member Accessors
public:

#ifdef PUGOPT_NONSEG

	//<summary>Access node name if any.</summary>
	//<returns>Name, or dummy value if the no name.</returns>
	//<remarks>Only returns up to 'PUGDEF_ELEM_NAME_SIZE' chars of name.</remarks>
	const TCHAR* name()
	{
		static TCHAR temp[PUGDEF_ELEM_NAME_SIZE] = {0};
		if(has_name())
		{
			_tcsncpy_s(temp,PUGDEF_ELEM_NAME_SIZE,_root->name,_root->name_size);
			temp[_root->name_size<PUGDEF_ELEM_NAME_SIZE?_root->name_size:(PUGDEF_ELEM_NAME_SIZE-1)] = 0;
			return temp;
		}
		return _T("");
	}
	unsigned int name_size() const { return (has_name()) ? _root->name_size : 0; } //Get node name length if any, else 0.
	unsigned int value_size() const { return (has_value()) ? _root->value_size : 0; } //Get node value length if any, else 0.
    // TODO: update all of these to compare lengths instead of using Max. Ditto for all uses of Max() NF 29 Mar 2003 See: xml_attribute_struct::matches_attribute_name_value()
	inline bool matches_attribute_name(const TCHAR* name,const unsigned int namelen,const int i) const { return (_tcsncmp(name,_root->attribute[i]->name,Max(namelen,_root->attribute[i]->name_size))==0); } //There is an attribute at 'i' named 'name'.
	inline bool matches_child_name(const TCHAR* name,const unsigned int namelen,const int i) const { return (_tcsncmp(name,_root->child[i]->name,Max(namelen,_root->child[i]->name_size))==0); } //There is a child at 'i' named 'name'.
	inline bool matches_name(const TCHAR* name,const unsigned int namelen,xml_node_struct* node) const { return (_tcsncmp(name,node->name,Max(namelen,node->name_size))==0); } //This is named 'name'.
	inline bool matches_value(const TCHAR* data,const unsigned int datalen,xml_node_struct* node) const { return (_tcsncmp(data,node->value,Max(datalen,node->value_size))==0); } //This is valued 'value'.
	inline bool matches_attribute_name(const TCHAR* name,const unsigned int namelen,xml_attribute_struct* attr) const { return (_tcsncmp(name,attr->name,Max(namelen,attr->name_size))==0); } //The given attribute is named 'name'.
	// inline bool matches_attribute_name_value(const TCHAR* value,const unsigned int valulen,xml_attribute_struct* attr) const { return (_tcsncmp(value,attr->value,Max(valulen,attr->value_size))==0); } //The given attribute is valued 'value'.
	inline bool matches_attribute_name_value(const TCHAR* value,const unsigned int valulen,xml_attribute_struct* attr) const { return attr->matches_attribute_name_value(value,valulen); } //The given attribute is valued 'value'.
#else
	const TCHAR* name(){ return (has_name()) ? _root->name : _T(""); } //Access pointer to node name if any, else empty string.
	inline bool matches_attribute_name(const TCHAR* name,const unsigned int i) const { return (strcmpwild(name,_root->attribute[i]->name)==0); } //There is an attribute at 'i' named 'name'.
	inline bool matches_child_name(const TCHAR* name,const unsigned int i) const { return (strcmpwild(name,_root->child[i]->name)==0); } //There is a child at 'i' named 'name'.
	inline bool matches_name(const TCHAR* name,xml_node_struct* node) const { return (strcmpwild(name,node->name)==0); } //This is named 'name'.
	inline bool matches_value(const TCHAR* data,xml_node_struct* node) const { return (strcmpwild(data,node->value)==0); } //This is valued 'value'.
	inline bool matches_attribute_name(const TCHAR* attribute,xml_attribute_struct* attr) const { return (strcmpwild(attribute,attr->name)==0); } //The given attribute is named 'name'.
 // inline bool matches_attribute_name_value(const TCHAR* value,xml_attribute_struct* attr) const { return (strcmpwild(value,attr->value)==0); } //The given attribute is valued 'value'.
	inline bool matches_attribute_name_value(const TCHAR* value,xml_attribute_struct* attr) const { return attr->matches_attribute_name_value(value); } //The given attribute is valued 'value'.
#endif
	xml_node_type type() const { return (_root) ? (xml_node_type)_root->type : node_null; } //Access node entity type.
	const TCHAR* value() const { return (has_value()) ? _root->value : _T(""); } //Access pointer to data if any, else empty string.

	//<summary>Get node name as const CEasyString. If no name, return empty.</summary>
	//<returns>The CEasyString node name, or empty string.</returns>
    //<since>NF 24 Mar 2003</since>
	const CEasyString get_name() const
	{
	    CEasyString temp;
		if(has_name())
		{
#ifdef PUGOPT_NONSEG
			temp.SetString(_root->name,_root->name_size);
#else
			temp = _root->name;
#endif
		}
		return temp;
	}

	//<summary>Get node value as const CEasyString. If no value, return empty.</summary>
	//<returns>The CEasyString node value, or empty string.</returns>
    //<since>NF 24 Mar 2003</since>
	const CEasyString get_value() const
	{
	    CEasyString temp;
		if(has_value())
		{
#ifdef PUGOPT_NONSEG
			temp.SetString(_root->value,_root->value_size);
#else
			temp = _root->value;
#endif
		}
		return temp;
	}

	unsigned int children() const { return _root->children; } //Access node's child count.
	xml_node child(unsigned int i){ return (i < children()) ? xml_node(_root->child[i]) : xml_node(); } //Access child node at subscript as xml_node or xml_node(NULL) if bad subscript.
	unsigned int attributes() const { return _root->attributes; } //Access node's attribute count.
	xml_attribute attribute(unsigned int i){ return (i < attributes()) ? xml_attribute(_root->attribute[i]) : xml_attribute(); } //Access attribute at subscript if any, else empty attribute.
	//<summary>Access or create the attribute having 'name'.</summary>
	//<param name="name">Name of attribute to access/create.</param>
	//<returns>Reference to xml_attribute wrapper.</returns>
	//Need Recovery xml_attribute attribute(const CEasyString& name){ return attribute(name.GetBuffer()); }
	//<summary>Access or create the attribute having 'name'.</summary>
	//<param name="name">Name of attribute to access/create.</param>
	//<returns>Reference to xml_attribute wrapper.</returns>
	xml_attribute attribute(const TCHAR* name)
	{
		xml_attribute_struct* attr = mapto_attribute_ptr(name);
		if(!attr) attr = append_attribute_origin(name,_T(""));
		return xml_attribute(attr);
	}

	unsigned int siblings() const { return (!type_document()) ? _root->parent->children : 0; } //Access node's sibling count (parent's child count).
	xml_node sibling(unsigned int i){ return (!type_document() && i < siblings()) ? xml_node(_root->parent->child[i]) : xml_node(); } //Access sibling node at subscript as xml_node or xml_node(NULL) if bad subscript.
	xml_node parent() const { return (!type_document()) ? xml_node(_root->parent) : xml_node(); } //Access node's parent if any, else xml_node(NULL)

	//<summary>Get the attribute having 'name'.</summary>
	//<param name="name">Name of attribute to access.</param>
	//<param name="def">Default value to use if the attribute doesn't exist.</param>
	//<returns>Reference to xml_attribute wrapper.</returns>
    //<since>2 Mar 2003 NF</since>
	xml_attribute getattribute(const TCHAR* name, const TCHAR* def = _T("") ) const
	{
		xml_attribute_struct* attr = mapto_attribute_ptr(name);
		if(!attr)
        {
			xml_attribute_struct* def_attr = MONITORED_NEW(_T("pugxml"), xml_attribute_struct);  // rem: dtor will delete def_attr.

                // We rely on 'def' being around for the life of the returned xml_attribute.
                // For typical use of: int Val = (int)some_node.getattribute( "someattr", "-1" );
                // thus is fine. If this proves inadequate then do what append_attribute() does.
            def_attr->value = const_cast<TCHAR*>( def );
#ifdef PUGOPT_NONSEG
            def_attr->value_size = _tcslen( def );
#endif
            def_attr->value_insitu = true;
            def_attr->name = NULL;   // we don't need the name for xml_attribute cast functions.
#ifdef PUGOPT_NONSEG
            def_attr->name_size = 0;
#endif
            def_attr->name_insitu = true;
		    return xml_attribute( def_attr, true );  // dtor will delete def_attr.
        }
		return xml_attribute(attr);
	}

	//<summary>Find the named attribute + value and return a pointer to that attribute, if found.</summary>
	//<param name="name">Reference to name of attribute to find.</param>
	//<param name="value>Reference to value of attribute to find, or NULL to match on any value.</param>
	//<returns>Pointer to attribute, or NULL if not found.</returns>
    //<since>31 Mar 2003 NF</since>
    xml_attribute_struct* findattribute( const TCHAR* name, const TCHAR* value ) const
    {
        xml_attribute_struct* pxa = mapto_attribute_ptr( name );
        bool bMatchFound = false;
        if ( pxa != NULL )  // 'name' found. note: We assume a given node has only one instance of a specific named attribute.
            if ( value == NULL )
                bMatchFound = true;     // all done, not checking attribute value
            else    // now check its value matches
#ifdef PUGOPT_NONSEG
            if ( pxa->matches_attribute_name_value( value, _tcslen( value ) ) )
#else
            if ( pxa->matches_attribute_name_value( value ) )
#endif
                bMatchFound = true;

	    return bMatchFound ? pxa : NULL;
    }

	//<summary>Return the first child that has data's data. If none, return empty string.</summary>
	//<returns>string containing the data. It will be empty if it doesn't exist.</returns>
	//<remarks>
	//	Used to get the PCDATA for the current element. This handles elements
	//	like: &lt;LINE&gt;&lt;STAGEDIR&gt;Aside&lt;/STAGEDIR&gt;Thy father,
	//	Pompey, would ne'er have&lt;/LINE&gt;, where 'this' points to &lt;LINE&gt;.
	//</remarks>
    //<since>3 Mar 2003 NF</since>
	const CEasyString child_value() const
    {
		CEasyString temp;
		if(_root->children)
		{
			for(register unsigned int i=0; i < _root->children; ++i)
			{
				xml_node_struct* node = _root->child[i];
				if(node->value)
				{
#ifdef PUGOPT_NONSEG
			        temp.SetString(node->value,node->value_size);
#else
			        temp = node->value;
#endif
                    break;
                }
            }
        }
		XMLDecodeString(temp);
        return temp;
    }

	//<summary>Return the first child that has data's data. If none, return NULL.</summary>
	//<param name="value">Returns a copy of the data.</param>
	//<param name="valuelen">Specifies the maximum number of characters to copy into value.</param>
	//<returns>Pointer to value if exists, else NULL.</returns>
	//<remarks>
	//	Used to get the PCDATA for the current element. This handles elements
	//	like: &lt;LINE&gt;&lt;STAGEDIR&gt;Aside&lt;/STAGEDIR&gt;Thy father,
	//	Pompey, would ne'er have&lt;/LINE&gt;, where 'this' points to &lt;LINE&gt;.
	//</remarks>
	TCHAR* child_value(TCHAR* value,const unsigned int valuelen)const
	{
		if(_root->children)
		{
			for(register unsigned int i=0; i < _root->children; ++i)
			{
				xml_node_struct* node = _root->child[i];
				if(node->value)
				{
					const unsigned int n =
#ifdef PUGOPT_NONSEG
						Min(valuelen,node->value_size);
#else
						Min(valuelen,_tcslen(node->value));
#endif
					_tcsncpy_s(value,valuelen,node->value,n);
					value[n] = 0;
					break;
				}
			}
			return value;
		}
		return NULL;
	}

//Name-To-Object Mapping
public:

	//<summary>Map an attribute name to a pointer to that attribute, if found.</summary>
	//<param name="name">Reference to name of attribute to find.</param>
	//<returns>Pointer to attribute, or NULL if not found.</returns>
	//<remarks>Implement your own hash table if you have a great many attributes.</remarks>
	//Need Recovery xml_attribute_struct* mapto_attribute_ptr(const CEasyString& name) const { return mapto_attribute_ptr(name.GetBuffer()); }

	//<summary>Map an attribute name to a pointer to that attribute, if found.</summary>
	//<param name="name">Pointer to name of attribute to find.</param>
	//<returns>Pointer to attribute, or NULL if not found.</returns>
	//<remarks>Implement your own hash table if you have a great many attributes.</remarks>
    //<see>findattribute( const TCHAR* name, const TCHAR* value )</see>
	xml_attribute_struct* mapto_attribute_ptr(const TCHAR* name) const
	{
		if(!_root || !name) return NULL;
		register unsigned int n = _root->attributes;
#ifdef PUGOPT_NONSEG
		const int namelen = _tcslen(name);
#endif
		for(register unsigned int i=0; i<n; ++i)
#ifdef PUGOPT_NONSEG
			if(matches_attribute_name(name,namelen,i))
#else
			if(matches_attribute_name(name,i))
#endif
				return _root->attribute[i];
		return NULL;
	}

	//<summary>Map an attribute name to the index of that attribute, if found.</summary>
	//<param name="name">Pointer to name of attribute to find.</param>
	//<returns>Index of attribute, or -1 if not found.</returns>
	//<remarks>Implement your own hash table if you have a great many attributes.</remarks>
	int mapto_attribute_idx(const TCHAR* name) const
	{
		if(!_root || !name) return -1;
		register unsigned int n = _root->attributes;
#ifdef PUGOPT_NONSEG
		const int namelen = _tcslen(name);
#endif
		for(register unsigned int i=0; i<n; ++i)
#ifdef PUGOPT_NONSEG
			if(matches_attribute_name(name,namelen,i))
#else
			if(matches_attribute_name(name,i))
#endif
				return i;
		return -1;
	}

	//<summary>Map a child name to a pointer to the first instance, if found.</summary>
	//<param name="name">Reference to name of child to find.</param>
	//<returns>Index of child, or -1 if not found.</returns>
	//<remarks>Implement your own hash table if you have a great many children.</remarks>
	//Need Recovery xml_node_struct* mapto_child_ptr(const CEasyString& name) const { return mapto_child_ptr(name.GetBuffer()); }

	//<summary>Map a child name to a pointer to the first instance, if found.</summary>
	//<param name="name">Pointer to name of child to find.</param>
	//<returns>Index of child, or -1 if not found.</returns>
	//<remarks>Implement your own hash table if you have a great many children.</remarks>
	xml_node_struct* mapto_child_ptr(const TCHAR* name) const
	{
		if(!_root || !name) return NULL;
		register unsigned int n = _root->children;
#ifdef PUGOPT_NONSEG
		const int namelen = _tcslen(name);
#endif
		for(register unsigned int i=0; i<n; ++i)
		{
			if
				(
				_root->child[i]->name &&
#ifdef PUGOPT_NONSEG
				matches_child_name(name,namelen,i)
#else
				matches_child_name(name,i)
#endif
				)
				return _root->child[i];
		}
		return NULL;
	}

	//<summary>Map a child name to the index of the first instance, if found.</summary>
	//<param name="name">Reference to name of child to find.</param>
	//<returns>Index of child, or -1 if not found.</returns>
	//<remarks>Implement your own hash table if you have a great many children.</remarks>
	//Need Recovery int mapto_child_idx(const CEasyString& name) const { return mapto_child_idx(name.GetBuffer()); }

	//<summary>Map a child name to the index of the first instance, if found.</summary>
	//<param name="name">Pointer to name of child to find.</param>
	//<returns>Index of child, or -1 if not found.</returns>
	//<remarks>Implement your own hash table if you have a great many children.</remarks>
	int mapto_child_idx(const TCHAR* name) const
	{
		if(!_root || !name) return -1;
		register unsigned int n = _root->children;
#ifdef PUGOPT_NONSEG
		const int namelen = _tcslen(name);
#endif
		for(register unsigned int i=0; i<n; ++i)
		{
			if
				(
				_root->child[i]->name &&
#ifdef PUGOPT_NONSEG
				matches_child_name(name,namelen,i)
#else
				matches_child_name(name,i)
#endif
				)
				return i;
		}
		return -1;
	}

//Traversal Helpers
public:

	//<summary>Find all elements having the given name.</summary>
	//<param name="name">Reference to name of child to find.</param>
	//<param name="found">Reference to xml_node_list or pointer_array to receive the matching elements.
	//Need Recovery void all_elements_by_name(const CEasyString& name,pointer_array& found) const { all_elements_by_name(name.GetBuffer(),found); }

	//<summary>Find all elements having the given name.</summary>
	//<param name="name">Pointer to name of child to find.</param>
	//<param name="found">Reference to xml_node_list or pointer_array to receive the matching elements.</param>
	void all_elements_by_name(const TCHAR* name,pointer_array& found) const
	{
		if(empty() || !name) return; //Invalid node, so fail.
		if(_root->children > 0) //Has children.
		{
#ifdef PUGOPT_NONSEG
			const unsigned int namelen = _tcslen(name);
#endif
			register unsigned int n = _root->children; //For each child.
			for(register unsigned int i=0; i<n; ++i)
			{
				if
					(
					_root->child[i] && //There is a child at i.
					_root->child[i]->name && //The child has a name.
#ifdef PUGOPT_NONSEG
					matches_child_name(name,namelen,i)
#else
					matches_child_name(name,i)
#endif
					)
					found.push_back(_root->child[i]); //push_back it to the array.
				if(_root->child[i]->children) //If there are children.
				{
					xml_node subsearch(_root->child[i]); //Wrap it up for ease.
					subsearch.all_elements_by_name(name,found); //Find any matching children.
				}
			}
		}
	}

	//<summary>
	//	Recursively-implemented depth-first find the first matching element.
	//	Use for shallow drill-downs.
	//</summary>
	//<param name="name">Const reference to name of element to find.</param>
	//<param name="type">Type of node to find, or node_null to match any node type.</param>
	//<returns>Valid xml_node if such element named 'name' is found.</returns>
	//<remarks>xml_node may be invalid if not found; test with 'empty'.</remarks>
	//Need Recovery xml_node first_element_by_name(const CEasyString& name,const xml_node_type type=node_null) const { return first_element_by_name(name.GetBuffer(),type); }

	//<summary>
	//	Recursively-implemented depth-first find the first matching element.
	//	Use for shallow drill-downs.
	//</summary>
	//<param name="name">Pointer to name of element to find.</param>
	//<param name="type">Type of node to find, or node_null to match any node type.</param>
	//<returns>Valid xml_node if such element named 'name' is found.</returns>
	//<remarks>xml_node may be invalid if not found; test with 'empty'.</remarks>
	xml_node first_element_by_name(const TCHAR* name,const xml_node_type type=node_null) const
	{
		if(empty() || !name) return xml_node(); //Invalid node, so fail.
		if(_root->children > 0) //Has children.
		{
			register unsigned int n = _root->children; //For each child.
#ifdef PUGOPT_NONSEG
			const int namelen = _tcslen(name);
#endif
			for(register unsigned int i=0; i<n; ++i)
			{
				if
				(   // check spec'd type matches or ignore if node_null. NF 25 Mar 2003
                    ( type==node_null
                   || _root->child[i]->type==type
                    ) &&
					_root->child[i]->name &&
#ifdef PUGOPT_NONSEG
					matches_child_name(name,namelen,i)
#else
					matches_child_name(name,i)
#endif
				)
					return xml_node(_root->child[i]);
				else
                if(_root->child[i]->children)
				{
					xml_node subsearch(_root->child[i]); //Wrap it up for ease.
					xml_node found = subsearch.first_element_by_name(name);
					if(!found.empty()) return found; //Found.
				}
			}
		}
		return xml_node(); //Not found.
	}

	//<summary>
	//	Recursively-implemented depth-first find the first matching element
	//	also having matching PCDATA.
	//</summary>
	//<param name="name">Reference to name of element to find.</param>
	//<param name="value">Reference to PCDATA to find.</param>
	//<returns>Valid xml_node if such element named 'name' is found with PCDATA 'value'.</returns>
	//<remarks>xml_node may be invalid if not found; test with 'empty'.</remarks>
	//Need Recovery xml_node first_element_by_value(const CEasyString& name,const CEasyString& value) const { return first_element_by_value(name.GetBuffer(),value.GetBuffer()); }

	//<summary>
	//	Recursively-implemented depth-first find the first matching element
	//	also having matching PCDATA.
	//</summary>
	//<param name="name">Pointer to name of element to find.</param>
	//<param name="value">Pointer to PCDATA to find.</param>
	//<returns>Valid xml_node if such element named 'name' is found with PCDATA 'value'.</returns>
	//<remarks>xml_node may be invalid if not found; test with 'empty'.</remarks>
	xml_node first_element_by_value(const TCHAR* name,const TCHAR* value) const
	{
		if(empty() || !name || !value) return xml_node(); //Invalid node, so fail.
		if(_root->children > 0) //Has children.
		{
			register unsigned int n = _root->children; //For each child.
#ifdef PUGOPT_NONSEG
			const unsigned int namelen = _tcslen(name);
			const unsigned int valulen = _tcslen(value);
#endif
			for(register unsigned int i=0; i<n; ++i)
			{
				if
				(
					_root->child[i] && //There is a child at i.
					_root->child[i]->name && //The child has a name.
#ifdef PUGOPT_NONSEG
					matches_child_name(name,namelen,i)
#else
					matches_child_name(name,i)
#endif
				)
				{
					register unsigned int m = _root->child[i]->children; //For each child of child.
					for(register unsigned int j=0; j<m; ++j)
					{
						if
						(
							_root->child[i]->child[j] && //There is a child at j.
							_root->child[i]->child[j]->type == node_pcdata && //It is of the PCDATA type.
							_root->child[i]->child[j]->value && //It has data.
#ifdef PUGOPT_NONSEG
							matches_value(value,valulen,_root->child[i]->child[j])
#else
							matches_value(value,_root->child[i]->child[j])
#endif
						)
							return xml_node(_root->child[i]); //Wrap it up and return.
					}
				}
				// else - this 'else' is wrong. If no match was found for this child
                //        we must descend down the tree. NF. 7 Oct 2003
                if(_root->child[i] && _root->child[i]->children) //The child has children.
				{
					xml_node subsearch(_root->child[i]); //Wrap it up for ease.
					xml_node found = subsearch.first_element_by_value(name,value); //Search any children.
					if(!found.empty()) return found; //Found.
				}
			}
		}
		return xml_node(); //Not found.
	}

	//<summary>
	//	Recursively-implemented depth-first find the first matching element
	//	also having matching attribute.
	//</summary>
	//<param name="name">Reference to name of element to find.</param>
	//<param name="attr_name">Reference to name of attribute to find.</param>
	//<param name="attr_value">Reference to attribute value to find.</param>
	//<param name="bDescend">true to descend down tree, false to onty do children.</param>
        //<since>bDescend NF 21 Sep 2003</since>
	//<returns>Valid xml_node if such element named 'name' is found.</returns>
	//<remarks>xml_node may be invalid if not found; test with 'empty'.</remarks>
	//Need Recovery xml_node first_element_by_attribute(const CEasyString& name,const CEasyString& attr_name,const CEasyString& attr_value,const bool bDescend=true)const { return first_element_by_attribute(name.GetBuffer(),attr_name.GetBuffer(),attr_value.GetBuffer(),bDescend); }

	//<summary>
	//	Recursively-implemented depth-first find the first matching element
	//	also having matching attribute.
	//</summary>
	//<param name="name">Pointer to name of element to find.</param>
	//<param name="attr_name">Pointer to name of attribute to find.</param>
	//<param name="attr_value">Pointer to attribute value to find.</param>
	//<param name="bDescend">true to descend down tree, false to onty do children.</param>
        //<since>bDescend NF 21 Sep 2003</since>
	//<returns>Valid xml_node if such element named 'name' is found.</returns>
	//<remarks>xml_node may be invalid if not found; test with 'empty'.</remarks>
    //<see>findattribute( const TCHAR* name, const TCHAR* value )</see>
	xml_node first_element_by_attribute(const TCHAR* name,const TCHAR* attr_name,const TCHAR* attr_value,const bool bDescend=true) const
	{
		if(empty() || !name || !attr_name || !attr_value) return xml_node(); //Invalid data, so fail.
		if(_root->children > 0) //Has children.
		{
#ifdef PUGOPT_NONSEG
			const unsigned int namelen = _tcslen(name);
			const unsigned int attrlen = _tcslen(attr_name);
			const unsigned int valulen = _tcslen(attr_value);
#endif
			register unsigned int n = _root->children; //For each child.
			for(register unsigned int i=0; i<n; ++i)
			{
				if
				(
					_root->child[i] && //There is a child at i.
					_root->child[i]->name && //The child has a name.
#ifdef PUGOPT_NONSEG
					matches_name(name,namelen,_root->child[i])
#else
					matches_name(name,_root->child[i])
#endif
				)
				{
					register unsigned int m = _root->child[i]->attributes; //For each attribute of child.
					for(register unsigned int j=0; j<m; ++j)
					{
						if
						(
							_root->child[i]->attribute[j] && //There is an attribute at j.
							_root->child[i]->attribute[j]->name && //The attribute has a name.
#ifdef PUGOPT_NONSEG
							matches_attribute_name(attr_name,attrlen,_root->child[i]->attribute[j]) &&
#else
							matches_attribute_name(attr_name,_root->child[i]->attribute[j]) &&
#endif
							_root->child[i]->attribute[j]->value && //The attribute has a value.
#ifdef PUGOPT_NONSEG
							matches_attribute_name_value(attr_value,valulen,_root->child[i]->attribute[j])
#else
							matches_attribute_name_value(attr_value,_root->child[i]->attribute[j])
#endif
						)
							return xml_node(_root->child[i]); //Wrap it up and return.
					}
				}
				// else - this 'else' is wrong. If no match was found for this child
                //        we must descend down the tree. NF. 7 Oct 2003
                if( bDescend && _root->child[i] && _root->child[i]->children )  // NF added bDescend 21 Sep 2003
				{
					xml_node subsearch(_root->child[i]); //Wrap it up for ease.
					xml_node found = subsearch.first_element_by_attribute(name,attr_name,attr_value,true); //Search any children.
					if(!found.empty()) return found; //Found.
				}
			}
		}
		return xml_node(); //Not found.
	}

	//<summary>
	//	Recursively-implemented depth-first find the first matching entity.
	//	Use for shallow drill-downs.
	//</summary>
	//<param name="name">Pointer to name of element to find.</param>
	//<returns>Valid xml_node if such element named 'name' is found.</returns>
	//<remarks>xml_node may be invalid if not found; test with 'empty'.</remarks>
	xml_node first_node(xml_node_type type) const
	{
		if(!_root) return xml_node();
		if(_root->children > 0) //Has children.
		{
			register unsigned int n = _root->children; //For each child.
			for(register unsigned int i=0; i<n; ++i)
			{
				if(_root->child[i]->type==type)
					return xml_node(_root->child[i]);
				else if(_root->child[i]->children)
				{
					xml_node subsearch(_root->child[i]);
					xml_node found = subsearch.first_node(type);
					if(!found.empty()) return found; //Found.
				}
			}
		}
		return xml_node(); //Not found.
	}

	//<summary>Move to the absolute root of the document tree.</summary>
	//<returns>True if the current node is valid.</returns>
	//<remarks>Member '_root' may now point to absolute root of the document.</remarks>
	bool moveto_root()
	{
		if(empty()) return false; //Nowhere to go.
		while(!type_document()) _root = _root->parent; //Keep stepping out until we hit the root.
		return true; //Success.
	}

	//<summary>Move to the current node's parent.</summary>
	//<returns>true if there is a parent and cursor is not parent, and cursor points thereto.</returns>
	//<remarks>'_root' may now point to parent.</remarks>
	bool moveto_parent()
	{
		if(empty() || type_document()) return false; //Invalid, or at the root (has no parent).
		if ( _root == _root->parent )
        {
            assert( false );   // something is terrible wrong!! NF 14 Oct 2003
		    return false;
        }
		_root = _root->parent; //Move to parent.
		return true; //Success.
	}

	//<summary>
	//	Move to the current node's sibling at subscript. Equivalent to
	//	'moveto_child' following 'moveto_parent'.
	//</summary>
	//<param name="i">Subscript of sibling to move cursor to.</param>
	//<returns>True if valid subscript, and cursor points thereto.</returns>
	//<remarks>If matching co-node was found, '_root' points thereto.</remarks>
	bool moveto_sibling(unsigned int i)
	{
		if(empty()) return false; //Nowhere to go.
		xml_node_struct* restore = _root; //Save position in case invalid subscript & we want to restore.
		if(moveto_parent()) //Try to move to parent.
		{
			if(i < children()) //Subscript is in range. (Assume parent *does* have children.)
			{
				_root = _root->child[i]; //Move to child at subscript ('sibling').
				return true; //Success.
			}
		}
		_root = restore; //Bad subscript, or parent move; restore.
		return false;
	}

	//<summary>Move to the current node's first sibling matching given name.</summary>
	//<param name="name">Element name of sibling to move to.</param>
	//<returns>True if sibling was found, and cursor points thereto.</returns>
	//<remarks>If matching co-node was found, '_root' points thereto.</remarks>
	//Need Recovery bool moveto_first_sibling(const CEasyString& name){ return moveto_first_sibling(name.GetBuffer()); }

	//<summary>Move to the current node's first sibling matching given name.</summary>
	//<param name="name">Element name of sibling to move to.</param>
	//<returns>True if sibling was found, and cursor points thereto.</returns>
	//<remarks>If matching co-node was found, '_root' points thereto.</remarks>
	bool moveto_first_sibling(const TCHAR* name)
	{
		if(empty() || !name) return false; //Nowhere to go, or nothing to find.
		xml_node_struct* restore = _root; //Save position in case invalid subscript & we want to restore.
		if(moveto_parent()) //Try to move to parent.
		{
#ifdef PUGOPT_NONSEG
			const unsigned int namelen = _tcslen(name);
#endif
			register unsigned int n = children(); //Search for matching name
			for(register unsigned int i=0; i<n; ++i)
			{
				//NF 24 Jan 2003 Changed to get child(i) just once per iteration.
				xml_node node = child(i); //Access child node at subscript as xml_node or xml_node(NULL) if bad subscript.
				if(node.type_element()||node.type_pi()) //Other types won't have names.
				{
#ifdef PUGOPT_NONSEG
					if(_tcsncmp(name,node.name(),Max(namelen,node.name_size()))==0) //Do names match?
#else
					if(strcmpwild(name,node.name())==0) //Do names match?
#endif
					{
						_root = node; //Move there.
						return true; //Success.
					}
				}
			}
		}
		_root = restore; //Failed to locate any such sibling; restore position.
		return false;
	}

	//<summary>Move to the current node's child at subscript.</summary>
	//<param name="i">Subscript of child to move cursor to.</param>
	//<returns>true if valid subscript, and cursor points thereto.</returns>
	//<remarks>If matching sub-node was found, '_root' points thereto.</remarks>
	bool moveto_child(unsigned int i)
	{
		if(empty()) return false; //Null, so no children.
		if(has_child_nodes() && i < children()) //Has children and subscript is in bounds.
		{
			_root = child(i); //Move to the child at i.
			return true; //Success.
		}
		return false; //Failure.
	}

	//<summary>Move to the current node's child matching given name.</summary>
	//<param name="name">Element name of child to move to if found.</param>
	//<returns>True if child was found, and cursor points thereto.</returns>
	//<remarks>If matching sub-node was found, '_root' points thereto.</remarks>
	//Need Recovery bool moveto_child(const CEasyString& name){ return moveto_child(name.GetBuffer()); }

	//<summary>Move to the current node's child matching given name.</summary>
	//<param name="name">Element name of child to move to if found.</param>
	//<returns>True if child was found, and cursor points thereto.</returns>
	//<remarks>If matching sub-node was found, '_root' points thereto.</remarks>
	bool moveto_child(const TCHAR* name)
	{
		if(empty() || !name || !has_child_nodes()) return false; //The node is null, a name was not specified, or node has no children.
#ifdef PUGOPT_NONSEG
		const unsigned int namelen = _tcslen(name);
#endif
		register unsigned int n = children(); //For each child.
		for(register unsigned int i=0; i<n; ++i)
		{
			//NF 24 Jan 2003: Changed to get child(i) just once per iteration.
			xml_node node = child(i); //Access child node at subscript as xml_node or xml_node(NULL) if bad subscript.
#ifdef PUGOPT_NONSEG
			if(_tcsncmp(name,node.name(),Max(namelen,node.name_size()))==0) //Do names match?
#else
			if(strcmpwild(name,node.name())==0) //If the name is identical with 'name'.
#endif
			{
				_root = node; //Move to it.
				return true; //Success.
			}
		}
		return false; //Failure.
	}

	//<summary>Move to the current node's next sibling by position and name.</summary>
	//<param name="name">Name of sibling to move to if found.</param>
	//<returns>True if there is a next sibling, and cursor points thereto.</returns>
	//Need Recovery bool moveto_next_sibling(const CEasyString& name){ return moveto_next_sibling(name.GetBuffer()); }

	//<summary>Move to the current node's next sibling by position and name.</summary>
	//<param name="name">Name of sibling to move to if found.</param>
	//<returns>True if there is a next sibling, and cursor points thereto.</returns>
	bool moveto_next_sibling(const TCHAR* name)
	{
		if(empty() || type_document() || !_root->parent || !name) return false; //Null, or at root, or no name, so there are no valid matches.
#ifdef PUGOPT_NONSEG
		const unsigned int namelen = _tcslen(name);
#endif
		register unsigned int n = _root->parent->children; //For each child of parent.
		for(register unsigned int i=0; i<(n-1); ++i)
		{
			if
			(
				_root->parent->child[i] && //There is a child at i.
				_root->parent->child[i] == _root && //The child is identical with this node.
				i < (n-1) //This is not the last child.
			)
			{
				for(++i; i<n; ++i) //For each following child.
				{
					if
					(
						_root->parent->child[i] && //There is a child at i.
						_root->parent->child[i]->name && //The child's name is not null.
#ifdef PUGOPT_NONSEG
						matches_name(name,namelen,_root->parent->child[i])
#else
						matches_name(name,_root->parent->child[i])
#endif
					)
					{
						moveto_sibling(i); //Move to it.
						return true; //Success.
					}
				}
			}
		}
		return false; //Failure.
	}

	//<summary>Move to the current node's next sibling by position.</summary>
	//<returns>True if there is a next sibling, and cursor points thereto.</returns>
	bool moveto_next_sibling()
	{
		if(empty() || type_document() || !_root->parent) return false; //Null or at root, so there are no valid siblings.
		register unsigned int n = _root->parent->children; //For each child of parent (each sibling).
		for(register unsigned int i=0; i<(n-1); ++i)
		{
			if
			(
				_root->parent->child[i] && //There is a child at i.
				_root->parent->child[i] == _root && //The child is identical with this node.
				i < (n-1) //This is not the last child.
			)
			{
				for(++i; i<n; ++i) //For each following child.
				{
					if(_root->parent->child[i]) //There is a child at i.
					{
						moveto_sibling(i); //Move to it.
						return true; //Success.
					}
				}
			}
		}
		return false; //Failure.
	}

	//<summary>Compile the absolute node path from root as a text string.</summary>
	//<param name="delimiter">Delimiter string to insert between element names.</param>
	//<returns>Path string (e.g. with '/' as delimiter, '/document/.../this'.</returns>
	CEasyString path(const TCHAR* delimiter = _T("/"))
	{
		TCHAR* path = NULL; //Current path.
		TCHAR* temp; //Temporary pointer.
		xml_node cursor = *this; //Make a copy.
#ifdef PUGOPT_NONSEG
		unsigned int destlen = 0;
		strcatgrown_impl(&path,cursor.name(),destlen,cursor.name_size()); //Get this name.
#else
		strcatgrow(&path,cursor.name()); //Get this name.
#endif
		while(cursor.moveto_parent() && !cursor.type_document()) //Loop to parent (stopping on actual root because it has no name).
		{
			temp = NULL; //Mark as null so 'strcatgrow' will allocate memory.
#ifdef PUGOPT_NONSEG
			destlen = 0;
			strcatgrown_impl(&temp,cursor.name(),destlen,cursor.name_size()); //Append next element name.
#else
			strcatgrow(&temp,cursor.name()); //Append next element name.
#endif
			strcatgrow(&temp,delimiter); //Append delimiter.
			strcatgrow(&temp,path); //Append current path.
			MONITORED_FREE(path); //Free the old path.
			path = temp; //Set path as new string.
		}
		temp = NULL;
		strcatgrow(&temp,delimiter); //Prepend final delimiter.
		strcatgrow(&temp,path); //Append current path.
		MONITORED_FREE(path); //Free the old path.
		CEasyString returns = temp; //Set path as new string.
		MONITORED_FREE(temp);
		return returns; //Return the path;
	}

	//<summary>Compile the absolute node path from root as a text string of xml_node_struct hex addresses..</summary>
	//<param name="delimiter">Delimiter string to insert between element names.</param>
	//<returns>Path string (e.g. with '/' as delimiter, '/A4EF/.../this'.</returns>
	//CEasyString pathaddress(const TCHAR* delimiter = _T("/"))
	//{
	//	xml_node cursor = *this; //Make a copy.
 //       char szTemp[ 50 ];
	//	_itoa_s( (int)((xml_node_struct*)cursor), szTemp,50, 16 );
	//	CEasyString spath(szTemp);

	//	while(cursor.moveto_parent() && !cursor.type_document()) //Loop to parent (stopping on actual root because it has no name).
	//	{
 //           _itoa_s( (int)((xml_node_struct*)cursor), szTemp,50, 16 );
 //           spath = CEasyString( szTemp ) + delimiter + spath;
	//	}
	//	return spath; //Return the path;
	//}

	//<summary>Search for a node by path.</summary>
	//<param name="path">
	//	Path string; e.g. './foo/bar' (relative to node), '/foo/bar' (relative
	//	to root), '../foo/bar' (pop relative position).
	//</param>
	//<param name="delimiter">Delimiter string to use in tokenizing path.</param>
	//<returns>Matching node, or xml_node(NULL) if not found.</returns>
	//Need Recovery xml_node first_element_by_path(const CEasyString& path,const CEasyString& delimiter = _T("/")){ return first_element_by_path(path.GetBuffer(),delimiter.GetBuffer()); }

	//<summary>Search for a node by path.</summary>
	//<param name="path">
	//	Path string; e.g. './foo/bar' (relative to node), '/foo/bar' (relative
	//	to root), '../foo/bar' (pop relative to position).
	//</param>
	//<param name="delimiter">Delimiter string to use in tokenizing path.</param>
	//<returns>Matching node, or xml_node(NULL) if not found.</returns>
	//<remarks>To-do: Support XPath-style queries.</remarks>
	xml_node first_element_by_path(const TCHAR* path,const TCHAR* delimiter = _T("/"))
	{
		if(!path) return xml_node();
		TCHAR* temp = NULL;
		TCHAR* NextPos=NULL;
		pointer_array path_segments; //Array of path segments.
		xml_node found = *this; //Current search context.
		strcatgrow(&temp,path);
		TCHAR* name = _tcstok_s(temp,delimiter,&NextPos);
		while(name) //Tokenize the whole path.
		{
			path_segments.push_back((void*)name); //push_back it to array.
			name = _tcstok_s(NULL,delimiter,&NextPos); //Get the next token,
		}
		register unsigned int n = path_segments.size();
		if(n == 0) return xml_node(); //Return null node if no path segments.
		if(path[0]==delimiter[0]) found.moveto_root(); //Absolute path; e.g. '/foo/bar'
		for(register unsigned int i = 0; i<n; ++i) //For each path segment.
		{
			name = (TCHAR*)path_segments.at(i);
			if(name)
			{
				if(*name==_T('.')) //Is '.' or '..'
				{
					if(_tcscmp(name,_T(".."))==0) found.moveto_parent(); //Pop.
					else continue; //Ignore '.' since it is redundant if path is './path'.
				}
				else
				{
					register unsigned int j, m = found.children(); //For each child.
					for(j=0; j<m; ++j)
					{
						if(found.child(j).has_name(name)) //Name matches?
						{
							found = found.child(j); //Move to this child.
							goto NEXT_ELEM; //Search next path segment.
						}
					}
					if(found.moveto_next_sibling(found.name())) //Find next sibling having same name.
					{
						if(i > 0) --i; //Try the previous path segment.
						goto NEXT_ELEM;
					}
					else //Move to parent to search further.
					{
						if(!found.type_document() && found.moveto_parent() && !found.type_document()) //Not root and stepped to parent and parent is not root.
						{
							if(i > 0) --i; //Try the previous path segment.
							if(found.moveto_next_sibling(found.name())) //Try to find next sibling having same name.
							{
								if(i > 0) --i; //Try the previous path segment.
								goto NEXT_ELEM;
							}
						}
					}
				}
			}
NEXT_ELEM:;
			if(found.type_document()) //Can't move up any higher, so fail.
			{
				MONITORED_FREE(temp); //Got to free this.
				return xml_node(); //Return null node.
			}
		}
		MONITORED_FREE(temp); //Got to free this.
		return found; //Return the matching node.
	}

	//<summary>Recursively traverse the tree. Depth first</summary>
	//<param name="walker">Reference to tree walker derived from xml_tree_walker.</param>
	//<returns>True if traversal was not halted by xml_tree_walker::for_each() callback.</returns>
	bool traverse(xml_tree_walker& walker)
	{
		if(walker.depth() == 0 && !walker.begin(*this)) return false; //Send the callback for begin traverse if depth is zero.
		if(!empty()) //Don't traveres if this is a null node.
		{
			walker.push(); //Increment the walker depth counter.
			register unsigned int n = _root->children; //For each child.
			for(register unsigned int i=0; i<n; ++i)
			{
				if(_root->child[i]) //There is a child at i.
				{
					xml_node subsearch(_root->child[i]); //Wrap it.
#ifdef ORIG  // 18 Sep 2003
					    if(!(walker.for_each(subsearch) && subsearch.traverse(walker)))
						    return false; //Traversal was aborted.
#else
                        bool bDescend = true;
					    if( !walker.for_each( subsearch, bDescend ) )
						    return false; //Traversal was aborted.

                        // descend down this branch if requested. . NF Added 18 Sep 2003
                        if ( bDescend && !subsearch.traverse( walker ) )
						    return false; //Traversal was aborted.
#endif
				}
			}
			walker.pop(); //Decrement the walker depth counter.
		}
		if(walker.depth() == 0 && !walker.end(*this)) return false; //Send the callback for end traverse if depth is zero.
		return true;
	}

//Editorial Helpers
public:

	//<summary>Set element name.</summary>
	//<param name="new_name">New element name.</param>
	//<returns>Success.</returns>
	//Need Recovery bool name(const CEasyString& new_name){ return name(new_name.GetBuffer()); }

	//<summary>Set element name.</summary>
	//<param name="new_name">New element name.</param>
	//<returns>Success.</returns>
	bool name(const TCHAR* new_name)
	{
		if((type_element() || type_pi()) && new_name)
#ifdef PUGOPT_NONSEG
			return strcpyinsitu(&_root->name,new_name,&_root->name_insitu,_root->name_size );
#else
			return strcpyinsitu(&_root->name,new_name,&_root->name_insitu);
#endif
		return false;
	}

	//<summary>Set node data.</summary>
	//<param name="value">New data (PCDATA, CDATA, or comment) value.</param>
	//<returns>Success.</returns>
	//Need Recovery bool value(const CEasyString& new_value){ return value(new_value.GetBuffer()); }

	//<summary>Set node data.</summary>
	//<param name="value">New data (PCDATA, CDATA, or comment) value.</param>
	//<returns>Success.</returns>
	bool value(const TCHAR* new_value)
	{
		if((type_pcdata() || type_cdata() || type_comment()) && new_value)
#ifdef PUGOPT_NONSEG
			return strcpyinsitu(&_root->value,new_value,&_root->value_insitu,_root->value_size);
#else
			return strcpyinsitu(&_root->value,new_value,&_root->value_insitu);
#endif
		return false;
	}

	//<summary>Remove attribute at the given subscript.</summary>
	//<param name="i">Subscript.</param>
	//<returns>Success.</returns>
	bool remove_attribute(unsigned int i)
	{
		unsigned int n = _root->attributes;
		if(i < n)
		{
			xml_attribute_struct* temp = _root->attribute[i];
			--n;
			for(unsigned int j=i; j<n; ++j)
				_root->attribute[j] = _root->attribute[j+1];
			_root->attribute[n] = NULL;
			if(!temp->name_insitu) MONITORED_FREE(temp->name);
			if(!temp->value_insitu) MONITORED_FREE(temp->value);
			MONITORED_FREE(temp);
			--_root->attributes;
			return true;
		}
		return false;
	}

	//<summary>Remove attribute having the given name.</summary>
	//<param name="name">Name of attribute to delete.</param>
	//<returns>Success.</returns>
	//Need Recovery bool remove_attribute(const CEasyString& name){ return remove_attribute(name.GetBuffer()); }

	//<summary>Remove attribute having the given name.</summary>
	//<param name="name">Name of attribute to delete.</param>
	//<returns>Success.</returns>
	bool remove_attribute(const TCHAR* name)
	{
		int i = mapto_attribute_idx(name);
		if(i > -1) return remove_attribute((unsigned int)i);
		return false;
	}

	//<summary>Append a new attribute to the node list of attributes.</summary>
	//<param name="name">Name.</param>
	//<param name="value">Value thereof.</param>
	//<returns>Attribute structure wrapper.</returns>
	//<remarks>Pointer space may be grown, memory for name/value members allocated.</remarks>
	//Need Recovery xml_attribute append_attribute(const CEasyString& name,const CEasyString& value){ return append_attribute(name.GetBuffer(),value.GetBuffer()); }

	//<summary>Append a new attribute to the node list of attributes.</summary>
	//<param name="name">Name.</param>
	//<param name="value">Value thereof.</param>
	//<returns>Attribute structure wrapper.</returns>
	//<remarks>Pointer space may be grown, memory for name/value members allocated.</remarks>
	xml_attribute append_attribute_origin(const TCHAR* name, const TCHAR* value)
	{
		if (!name || !value)
			return xml_attribute(); //We must have both to proceed.

		xml_attribute_struct* p = pug::append_attribute(_root,1); //Append/allocate a new attribute structure.
		if(p) //If append/allocate succeeded.
		{
#ifdef PUGOPT_NONSEG
			strcatgrown(&p->name,name,p->name_size); //Append the name.
			strcatgrown(&p->value,value,p->value_size); //Append the name.
#else
			strcatgrow(&p->name,name); //Append the name.
			strcatgrow(&p->value,value); //Append the name.
#endif
			p->name_insitu = p->value_insitu = false; //Mark as not part of original parse string.
			return xml_attribute(p); //Success.
		}
		return xml_attribute(); //Failure; return an empty.
	}

	xml_attribute append_attribute(const TCHAR* name, const TCHAR* value)
	{
		CEasyString Str = XMLEncodeString(value);

		return append_attribute_origin(name, Str);
	}

	//<summary>Append a new attribute of type int to the node list of attributes.</summary>
	//<param name="name">Name.</param>
	//<param name="value">Value thereof.</param>
	//<returns>Attribute structure wrapper.</returns>
	//<remarks>Pointer space may be grown, memory for name/value members allocated.</remarks>

	xml_attribute append_attribute(const TCHAR* name, char value)
	{
		if (!name) return xml_attribute();
		TCHAR temp[32] = { 0 };
		_stprintf_s(temp, 32, _T("%d"), value);
		return append_attribute_origin(name, temp);
	}

	xml_attribute append_attribute(const TCHAR* name, unsigned char value)
	{
		if (!name) return xml_attribute();
		TCHAR temp[32] = { 0 };
		_stprintf_s(temp, 32, _T("%u"), value);
		return append_attribute_origin(name, temp);
	}

	xml_attribute append_attribute(const TCHAR* name, short value)
	{
		if (!name) return xml_attribute();
		TCHAR temp[32] = { 0 };
		_stprintf_s(temp, 32, _T("%d"), value);
		return append_attribute_origin(name, temp);
	}

	xml_attribute append_attribute(const TCHAR* name, unsigned short value)
	{
		if (!name) return xml_attribute();
		TCHAR temp[32] = { 0 };
		_stprintf_s(temp, 32, _T("%u"), value);
		return append_attribute_origin(name, temp);
	}

	xml_attribute append_attribute(const TCHAR* name,int value)
	{
		if (!name) return xml_attribute();
		TCHAR temp[32] = {0};
		_stprintf_s(temp,32,_T("%d"),value);
		return append_attribute_origin(name, temp);
	}

	xml_attribute append_attribute(const TCHAR* name,unsigned int value)
	{
		if (!name) return xml_attribute();
		TCHAR temp[32] = {0};
		_stprintf_s(temp,32,_T("%u"),value);
		return append_attribute_origin(name, temp);
	}

	xml_attribute append_attribute(const TCHAR* name, __int64 value)
	{
		if (!name) return xml_attribute();
		TCHAR temp[32] = { 0 };
		_stprintf_s(temp, 32, _T("%lld"), value);
		return append_attribute_origin(name, temp);
	}

	xml_attribute append_attribute(const TCHAR* name, unsigned __int64 value)
	{
		if (!name) return xml_attribute();
		TCHAR temp[32] = { 0 };
		_stprintf_s(temp, 32, _T("%llu"), value);
		return append_attribute_origin(name, temp);
	}

	//xml_attribute append_attribute(const TCHAR* name,int value)
	//{
	//	if(!name) return xml_attribute();
	//	TCHAR temp[32] = {0};
	//	_stprintf_s(temp,32,_T("%d"),value);
	//	return append_attribute(name,temp);
	//}

	//xml_attribute append_attribute(const TCHAR* name,unsigned int value)
	//{
	//	if(!name) return xml_attribute();
	//	TCHAR temp[32] = {0};
	//	_stprintf_s(temp,32,_T("%d"),value);
	//	return append_attribute(name,temp);
	//}

	//<summary>Append a new attribute of type double to the node list of attributes.</summary>
	//<param name="name">Name.</param>
	//<param name="value">Value thereof.</param>
	//<returns>Attribute structure wrapper.</returns>
	//<remarks>Pointer space may be grown, memory for name/value members allocated.</remarks>

	xml_attribute append_attribute(const TCHAR* name, float value)
	{
		if (!name) return xml_attribute();
		TCHAR temp[32] = { 0 };
		_stprintf_s(temp, 32, _T("%lf"), value);
		return append_attribute_origin(name, temp);
	}

	xml_attribute append_attribute(const TCHAR* name,double value)
	{
		if (!name) return xml_attribute();
		TCHAR temp[32] = {0};
		_stprintf_s(temp,32,_T("%lf"),value);
		return append_attribute_origin(name, temp);
	}

	//<summary>Append a new attribute of type bool to the node list of attributes.</summary>
	//<param name="name">Name.</param>
	//<param name="value">Value thereof.</param>
	//<returns>Attribute structure wrapper.</returns>
	//<remarks>Pointer space may be grown, memory for name/value members allocated.</remarks>
	xml_attribute append_attribute(const TCHAR* name,bool value)
	{
		if (!name) return xml_attribute();
		return append_attribute_origin(name, ((value) ? _T("true") : _T("false")));
	}

	//<summary>Set the current node entity type.</summary>
	//<param name="new_type">New type to set.</param>
	//<returns>Previous type.</returns>
	//<remarks>If has children and now is not node_element, children are obscured.</remarks>
	xml_node_type type(xml_node_type new_type)
	{
		xml_node_type prev = _root->type; //Save old type.
		_root->type = new_type; //Set new type.
		return prev; //Return old type.
	}

    //====================


	//<summary>Finds the first child node of the given type.</summary>
	//<param name="type">Child node type to find.</param>
	//<returns>Ptr to child node if found, else NULL.</returns>
	//<remarks>Typically used to find the PCDATA node for an ELEMENT.</remarks>
    //<since>25 June 2003 NF</since>
 	xml_node_struct* find_child( const xml_node_type type ) const
    {
		if( _root->children )
		{
			for(register unsigned int i=0 ; i < _root->children ; ++i )
			{
				xml_node_struct* node = _root->child[i];
				if( node->type == type )
                    return node;
            }
        }
        return NULL;
    }

	//<summary>Finds the specified child node.</summary>
	//<param name="type">xml_node_struct* to find.</param>
	//<returns>child[index] if found, else -1.</returns>
    //<since>26 June 2003 NF</since>
 	unsigned int find_child( const xml_node_struct* xns ) const
    {
		if( _root->children )
		{
			for( unsigned int i = 0 ; i < _root->children ; ++i )
			{
				if ( _root->child[i] == xns )
                    return i;
            }
        }
        return -1;
    }


	//<summary>
	//	Sets the value for the first child of this node, appending a child node
    //  of the given type if required. Typically used to set the PCDATA for a node.
	//</summary>
	//<param name="type">New child node type.</param>
	//<param name="new_name">New element value.</param>
	//<returns>xml_node wrapping the new child. Use empty() to test for success.</returns>
	//<remarks>Pointer space may be grown. An xml_node_struct structure maybe allocated.</remarks>
    //<since>4 Mar 2003 NF</since>
	//Need Recovery xml_node set_child_value(xml_node_type type, const CEasyString& new_value)
	//Need Recovery     { return set_child_value(type, new_value.GetBuffer()); }

	//<summary>
	//	Sets the value for the first child of this node, appending a child node
    //  of the given type if required. Typically used to set the PCDATA for a node.
	//</summary>
	//<param name="type">New child node type.</param>
	//<param name="new_name">New element value. NULL clears the value.</param>
	//<returns>xml_node wrapping the new child. Use empty() to test for success.</returns>
	//<remarks>Pointer space may be grown. An xml_node_struct structure maybe allocated.</remarks>
    //<since>4 Mar 2003 NF</since>
	xml_node set_child_value(xml_node_type type, const TCHAR* new_value)
    {
        if ( children() == 0 )
            append_child( type );
        xml_node node_value = child( 0 );
		if (!node_value.empty())
		{
			CEasyString Value = XMLEncodeString(new_value);

			if (!node_value.value(Value))
				return xml_node();    // empty() node.
		}
        return node_value;     // rem: will be empty() if an error occured.
    }

	//<summary>
	//	Allocate & append a child node of the given type and name at the
	//	end of the current node array of children.
	//</summary>
	//<param name="type">New child node type.</param>
	//<param name="new_name">New element name.</param>
	//<returns>xml_node wrapping the new child. Use empty() to test for success.</returns>
	//<remarks>Pointer space may be grown. An xml_node_struct structure is allocated.</remarks>
    //<since>4 Mar 2003 NF</since>
	//Need Recovery xml_node append_child(xml_node_type type, const CEasyString& new_name)
    //Need Recovery         { return append_child(type,new_name.GetBuffer()); }

	//<summary>
	//	Allocate & append a child node of the given type and name at the
	//	end of the current node array of children.
	//</summary>
	//<param name="type">New child node type.</param>
	//<param name="new_name">New element name.</param>
	//<returns>xml_node wrapping the new child. Use empty() to test for success.</returns>
	//<remarks>Pointer space may be grown. An xml_node_struct structure is allocated.</remarks>
    //<since>4 Mar 2003 NF</since>
	xml_node append_child(xml_node_type type, const TCHAR* new_name)
    {
        xml_node xml_new_node = append_child(type);
        if ( !xml_new_node.empty() )
            if ( !xml_new_node.name( new_name ) )
            {       // remove the child we just added.
                remove_child(children()-1);
                return xml_node();  // empty node.
            }
        return xml_new_node;
    }

	//<summary>
	//	Allocate & append a child node of the given type at the end of the
	//	current node array of children.
	//</summary>
	//<param name="type">New child node type.</param>
	//<returns>xml_node wrapping the new child.</returns>
	//<remarks>Pointer space may be grown. An xml_node_struct structure is allocated.</remarks>
	xml_node append_child(xml_node_type type)
	{
		if(type_document()||type_element()) //Don't do anything if not an node_element or root.
		{
			xml_node_struct* p = pug::append_node(_root,1,type); //Append the node.
			if(p)
			{
				p->name_insitu = p->value_insitu = false;     // new_node() sets these to true.
				return xml_node(p); //If we have it, return wrapped.
			}
		}
		return xml_node(); //Return dummy.
	}

	//<summary>Allocate & insert a child node of the given type at subscript.</summary>
	//<param name="i">Subscript at which to insert.</param>
	//<param name="type">New child node type. n/u if xns_child is specified.</param>
    //<param name="xns_child">Child node to append or NULL for new node.</param>
    //<since="xns_child">26 Jun 2003</since>
	//<returns>xml_node wrapping the new child.</returns>
	//<remarks>
	//	Pointer space may be grown. An xml_node_struct structure is allocated,
	//	and existing children are shifted in their array position.
	//</remarks>
	xml_node insert_child( const unsigned int i, const xml_node_type type, xml_node_struct* xns_child = NULL )
	{
		if( type_element() ) // Don't do anything if not an node_element.
        {
		    unsigned int n = _root->children; //Get count of existing children.
#ifdef ORIG_CODE
		    if( i >= n )
                return append_child(type); //If subscript at end of array then just append.
		    else
            if( i < n )
		    {
#endif
                // Append the new node (by default at last array position).
			    xml_node_struct* p = pug::append_node( _root, 1, type, xns_child );
			    if( p ) //Ensure we have it.
			    {   // If subscript before end of array then shift it back from
                    // the end to its spec'd location.
                    if( i < n )
                    {
				        register int m = (i-1); //Stop at i.
				        for(register int j=(n-1); j>m; --j) //Starting at one less than end of array, reverse loop to i.
					        _root->child[j+1] = _root->child[j]; //Shift node to right.
				        _root->child[i] = p; //Set node at subscript to new node.
                    }

                    if ( xns_child == NULL )
				        p->name_insitu = p->value_insitu = false;   // new_node() sets these to true. Do same as append_child(). NF 26 Jun 2003

				    return xml_node( p ); // Return new node.
			    }
#ifdef ORIG_CODE
		    }
#endif
        }
		return xml_node(); // Return dummy.
	}

	//<summary>Remove all child nodes of the given type.</summary>
	//<param name="type">Child node type to remove.</param>
	//<returns>True if any child nodes removed.</returns>
    //<since>4 Mar 2003 NF</since>
	//<remarks>Shifts child array element positions. Frees entire tree under deleted children.</remarks>
	bool remove_child(xml_node_type type)
    {
        bool bStat = false;
		if(_root->children)
		{
			for(register unsigned int i=0; i < _root->children; )
			{
				xml_node_struct* node = _root->child[i];
				if(node->type == type)
				{
                    remove_child( i );   // rem: i -> next child after this, so don't increment.
                    bStat = true;        // one or more children removed.
                }
                else
                    ++i;
            }
        }
        return bStat;
    }

	//<summary>Delete the child node at the given subscript.</summary>
	//<param name="i">Subscript.</param>
	//<returns>Success.</returns>
	//<remarks>Shifts child array element positions. Frees entire tree under child to be deleted.</remarks>
	bool remove_child( const unsigned int i )
	{
#ifdef ORIG_CODE
		unsigned int n = _root->children;
		if(i < n) //Ensure subscript is in bounds.
		{
			xml_node_struct* p = _root->child[i]; //Keep a pointer to this node so we can free it.
			--n;
			for(unsigned int j=i; j<n; ++j) //Shift everything left from this point on.
				_root->child[j] = _root->child[j+1];
			_root->child[j] = NULL; //Mark the last element null.
			--_root->children; //One less children.
			p->parent = p; //This ensures we only free this node when calling 'free_node'.
			pug::free_node(p); //Free the node tree.
			return true; //Success.
		}
		return false; //Failure.
#else
        // Use new disconnect_child() NF 26 Jun 2003
	    xml_node_struct* p = disconnect_child( i );
        if ( p != NULL )
        {
			p->parent = p;      // This ensures we only free this node when calling 'free_node'.
			pug::free_node(p);  // Free the node tree.
        }
        return p != NULL;
#endif
	}

	//<summary>Disconnect the child node at the given subscript.</summary>
	//<param name="i">Subscript.</param>
	//<returns>xml_node_struct* for specified child if successfull, else NULL.</returns>
	//<remarks>Shifts child array element positions.</remarks>
	xml_node_struct* disconnect_child( const unsigned int i )
	{
        xml_node_struct* p = NULL;
		unsigned int n = _root->children;
		if(i < n) //Ensure subscript is in bounds.
		{
			p = _root->child[i]; // Keep a pointer to this node so we can return it.
			--n;
			unsigned int j=i;
			for( ; j<n; ++j ) // Shift everything left from this point on.
				_root->child[j] = _root->child[j+1];
			_root->child[j] = NULL; // Mark the last element null.
			--_root->children;      // One less children.
		}
		return p;
	}


	//Stream/Output Helpers
public:

	//<summary>
	//	Stream output. Recursively writes the internal xml_node_struct structure
	//	to the given stream.
	//</summary>
	//<param name="os">Reference to output stream.</param>
	//<param name="indent_char">Char to use for indent.</param>
	//<param name="breaks">Use linebreaks?</param>
	//<remarks>String data is written to stream.</remarks>
	void outer_xml(std::basic_ostream<TCHAR,std::char_traits<TCHAR> >& os,TCHAR indent_char = _T('\t'),bool breaks = true) const
	{
		if(empty()) return; //Make sure there is something to output.
		indent_stack indent(indent_char); //Prepare the indent.
		if(type_document()) //If this is the root, we don't want to output the root itself.
		{
			register unsigned int n = _root->children; //Output each child of the root.
			for(register unsigned int i=0; i<n; ++i)
				pug::outer_xml(os,indent,_root->child[i],breaks);
		}
		else pug::outer_xml(os,indent,_root,breaks); //Output the node.
	}

	//<summary>
	//	Stream output operator. Wraps 'outer_xml'. Recursively writes
	//	the given node to the given stream.
	//</summary>
	//<param name="os">Reference to output stream.</param>
	//<param name="xml_node">Reference to tree node.</param>
	//<returns>Reference to output stream.</returns>
	//<remarks>String data is written to stream.</remarks>
	friend std::basic_ostream<TCHAR,std::char_traits<TCHAR> >& operator<<(std::basic_ostream<TCHAR,std::char_traits<TCHAR> >& os,xml_node node)
	{
		if(!os.good()) return os;
		if((os.flags()|std::ostream::skipws) == std::ostream::skipws)
			node.outer_xml(os,0,false); //Skipping whitespace; suppress indents & linebreaks.
		else node.outer_xml(os); //Default options.
		return os;
	}

	// XML Conversion Helpers. NF 31 Mar 2003
public:
    // <summary>
    //    Convert any entity references for special characters to their normal Ascii
    //    character. Also replaces entity references with their DTD entity-values.
    // </summary>
    // <param name="sxml_pcdata">xml text to convert.</param>
    // <remarks>
    //    'this' node must be the DTD node otherwise Entity reference lookup will fail.
    //    To get the DTD use xml_parser->document().first_node( node_doctype );
    //    A DTD is required with <!ENTITY ..> entries for each entity reference you want
    //    handled.
    //    Hex and Decimal entity references "&#x...;" and "&#..." are also converted.
    //    A DTD is not required for these.
    //    See also: <A HREF=".\str_markup.cpp#^xml_to_text"> which strips tags
    //              and doesn't require a DTD.
    // </remarks>
    // <since>NF 31 Mar 2003</since>
    // <returns>string with converted Ascii text.</returns>
    CEasyString xml_to_text( const CEasyString& sxml_pcdata ) const
    {
        assert( type_doctype() );  // must be DTD - see cmt above.

	    CEasyString sxml_value( sxml_pcdata );
		int idx = 0;
        do  // parse sxml_value
        {       // find an entity reference "&...;"
            idx = sxml_value.Find( '&', idx );
            int idxe = sxml_value.Find( ';', idx );
			if ( idx != CEasyString::ERROR_CHAR_POS
              && idxe != CEasyString::ERROR_CHAR_POS
               )
            {       // get the text in between "&...;"
                const int len = ( idxe - idx ) - 1;
                CEasyString token = sxml_value.SubStr( ++idx, len );  // rem: incr past the '&'

                    // if there is a DTD Entity for this referernce use its value
                xml_node dtdentity;
                if ( type_doctype() )    // don't waist time searching if 'this' isn't the DTD node
                    dtdentity = first_element_by_name(token,node_dtd_entity);
                if ( !dtdentity.empty() )
                {       // get the entity value, stripping off the surrounding quotes.
                    CEasyString entvalue = dtdentity.get_value().SubStr( 1, dtdentity.get_value().GetLength()-2 );
                    token = entvalue;

                        // Start by seeing if the entity-value is a simple replacement character.

                        // &lt; and &amp; use special case values with a leading &#38 (&)
                    if ( token.Compare( 0, 5, _T("&#38;") ) == 0 )
                    {
                        token.Delete( 0, 4 );     // remove "&#38"
                        token[0] = '&';          // change ; to # so we have "&#..."
                    }
                        // if the entity-value is an Ascii character value
                    if ( token.Compare( 0, 2, _T("&#") ) == 0 )
                    {
                        // if the entity-value represents more than a single replacement
                        // character we must use the entire entity-value as the replacement.
                        // eg. <!ENTITY  test "&#x0a;&lf;line1 with amp &amp;&#x0a;&quot;line2&quot;&lf;&lt;line3&gt;">
                        // Note that Entities like: <!ENTITY amp "&#38;#38;"> complicate things.
						int idxsc = token.Find( ';' );      // see if > 1 ";"
                        if ( idxsc == CEasyString::ERROR_CHAR_POS || idxsc == (int)token.GetLength()-1 )
                        {
                            token = token.SubStr( 1,-1 );  // skip past "&"
                            goto cvt_to_char;
                        }
                    }

                    // replace the entity reference with the spec'd text.
                    sxml_value.Replace( idx-1, len+2, entvalue );
                    --idx;   // continue from where we just replaced.
                }
                else  // if "&#x...;" or "&#..." numeric value spec'd
                if ( token[0] == '#' )
                {
cvt_to_char:        // if hex "&#x.." change to 0x.. for _tcstol()
                    if ( token[1] == 'x' )
                        token[0] = '0';
                    else
                        token.Delete( 0, 1 );   // remove #

                    char ch = (char)_tcstol( token.GetBuffer(), NULL, 0 );     // rem: base=0 to auto-determine base
                    sxml_value.Replace( idx-1, len+2, 1, ch );
                }
            }
        }
        while( idx != CEasyString::ERROR_CHAR_POS );

        return sxml_value;
    }

};


//<summary>Provides a high-level interface to the XML parser.</summary>
class xml_parser
{
//Internal Data Members
protected:

	int					m_LocalCodePage;

	xml_node_struct*	_xmldoc; //Pointer to current XML document tree root.
	int				_growby; //Attribute & child pointer space growth increment.
	bool				_autdel; //Delete the tree on destruct?
	CEasyString			_buffer; //Pointer to in-memory buffer (for 'parse_file').
	TCHAR*				_strpos; //Where parsing left off (for 'parse_file').
	unsigned int		_optmsk; //Parser options.
#ifdef PUGOPT_MEMFIL
	HANDLE				_mmfile; //File handle.
	HANDLE				_mmfmap; //Handle which maps the file.
	void*				_mmaddr; //Base address of map.
	size_t				_mfsize; //Size of memory-mapped file.
	bool				_addeos; //True if we had to add a 0 to then end of the file.
#endif
#ifdef PUGOPT_NODE_FLAGS
public:
    vector<xml_node_struct*> _expanded_nodes;  // list of expanded nodes found during parse(). NF 29 May 2003
#endif

//Construction/Destruction
public:



	//<summary>Constructor.</summary>
	//<param name="optmsk">Options mask.</param>
	//<param name="autdel">Delete tree on destruct?</param>
	//<param name="growby">Parser pointer space growth increment.</param>
	//<remarks>Root node structure is allocated.</remarks>
	xml_parser(unsigned int optmsk = parse_default,bool autdel = true,int growby = parse_grow):
		m_LocalCodePage(CP_ACP),
		_xmldoc(0),
			_growby(growby),
			_autdel(autdel),
			_optmsk(optmsk),
			_strpos(0)
#ifdef PUGOPT_MEMFIL
			,
			_mmfile(0),
			_mmfmap(0),
			_mmaddr(0),
			_mfsize(0),
			_addeos(false)
#endif
		{
		}

		//<summary>Direct parse constructor.</summary>
		//<param name="xmlstr">
		//	XML-formatted string to parse. Note: String must persist for the
		//	life of the tree. String is zero-segmented, but not freed.
		//</param>
		//<param name="optmsk">Parser options.</param>
		//<param name="autdel">Delete tree on destruct?</param>
		//<param name="growby">Parser pointer space growth increment.</param>
		//<remarks>Root node structure is allocated, string is parsed & tree may be grown.</remarks>
		xml_parser(TCHAR* xmlstr,unsigned int optmsk = parse_default,bool autdel = true,int growby = parse_grow) :
			m_LocalCodePage(CP_ACP),
			_xmldoc(0),
			_growby(growby),
			_autdel(autdel),
			_optmsk(optmsk),
			_strpos(0)
#ifdef PUGOPT_MEMFIL
			,
			_mmfile(0),
			_mmfmap(0),
			_mmaddr(0),
			_mfsize(0),
			_addeos(false)
#endif
		{
			parse( xmlstr, _optmsk ); //Parse it.
		}

		//<summary>Destructor.</summary>
		//<remarks>Tree memory and string memory may be freed.</remarks>
		virtual ~xml_parser()
		{
			if(_autdel && _xmldoc) free_node(_xmldoc);
			_buffer.Clear();


		}

//Accessors/Operators
public:

	operator xml_node_struct*() { return _xmldoc; } //Cast as xml_node_struct pointer to root.
	operator xml_node() { return xml_node(_xmldoc); } //Cast as xml_node (same as document).
	xml_node document(){ return xml_node(_xmldoc); } //Returns the root wrapped by an xml_node.

//Miscellaneous
public:

	//<summary>Allocate a new, empty root.</summary>
	//<remarks>Tree memory and string memory may be freed.</remarks>
	void create()
	{
		clear(); //Free any allocated memory.
		_xmldoc = new_node(node_document); //Allocate a new root.
		_xmldoc->parent = _xmldoc; //Point to self.
	}

	//<summary>Clear any existing tree or string.</summary>
	//<remarks>Tree memory and string memory may be freed.</remarks>
	void clear()
	{
		if(_xmldoc){ free_node(_xmldoc); _xmldoc = 0; }
		_buffer.Clear();

	}

	void SetLocalCodePage(int CodePage)
	{
		m_LocalCodePage = CodePage;
	}

	int GetLocalCodepage()
	{
		return m_LocalCodePage;
	}

#ifdef PUGOPT_MEMFIL

//Memory-Mapped File Support
protected:




public:

#endif

	//<summary>Attach an externally-generated root to the parser.</summary>
	//<param name="root">Pointer to node structure.</param>
	//<returns>Pointer to old root if any.</returns>
	//<remarks>New root may be deleted on dtor if autodelete set.</remarks>
	xml_node_struct* attach(xml_node_struct* root)
	{
		xml_node_struct* t = _xmldoc; //Save this root.
		_xmldoc = root; //Assign.
		_xmldoc->parent = _xmldoc; //Ensure we are the root.
		return t; //Return the old root if any.
	}

	//<summary>Detach the current root from the parser.</summary>
	//<returns>Pointer to old root, if any.</returns>
	xml_node_struct* detach()
	{
		xml_node_struct* t = _xmldoc; //Save this root.
		_xmldoc = 0; //So we don't delete later on if autodelete set.
		return t; //Return the old root if any.
	}

	//<summary>Get parser optsions mask.</summary>
	//<returns>Options mask.</returns>
	unsigned int options(){ return _optmsk; }

	//<summary>Set parser options mask.</summary>
	//<param name="optmsk">Options mask to set.</param>
	//<returns>Old options mask.</returns>
	unsigned int options(unsigned int optmsk)
	{
		unsigned int prev = _optmsk;
		_optmsk = optmsk;
		return prev;
	}

	//<summary>Get pointer space growth size increment.</summary>
	//<returns>Grow size.</returns>
	unsigned int growby(){ return _growby; }

	//<summary>Set pointer space growth size increment.</summary>
	//<param name="grow">Grow size to set.</param>
	//<returns>Old size.</returns>
	unsigned int growby(int grow)
	{
		int prev = _growby;
		_growby = grow;
		return prev;
	}

	//<summary>Get parse file buffer last string position.</summary>
	//<returns>Last string position.</returns>
	//<remarks>
	//	Use after parse_file, with parse_dtd_only set in order to recommence
	//	parse of document body.
	//</remarks>
	TCHAR* strpos()
	{
		return _strpos;
	}

//Parsing Helpers
public:

	//<summary>Parse the given XML string in-situ.</summary>
	//<param name="s">Pointer to XML-formatted string.</param>
	//<param name="optmsk">Parser options mask.</param>
	//<returns>Last string position or null.</returns>
	//<remarks>Input string is zero-segmented.</remarks>
	TCHAR* parse( TCHAR* s, unsigned int optmsk = parse_noset )
	{
		if(!s) return s;
		clear(); //Free any allocated memory.
		_xmldoc = new_node(node_document); //Allocate a new root.
		_xmldoc->parent = _xmldoc; //Point to self.
		if(optmsk != parse_noset) _optmsk = optmsk;
		return parse( s, _xmldoc, _growby, _optmsk ); // Parse the input string.
		// return pug::parse( s, _xmldoc, _growby, _optmsk ); //Parse the input string.
	}

	//<summary>Load into memory and parse the contents of the file at the given path.</summary>
	//<param name="path">File path.</param>
	//<param name="optmsk">Parser options.</param>
        //<param name="tempsize">Temporary read buffer size.</param>
	//<returns>Success if the file was loaded.</returns>
	//<remarks>
	//	The file contents is loaded and stored in the member '_buffer' until
	//	freed by calling 'Parse', 'parse_file', 'clear' or '~xml_parser'.
	//</remarks>
	bool parse_file(const TCHAR* path, unsigned int optmsk = parse_noset, int FileChannel = FILE_CHANNEL_NORMAL1);
	void SaveToString(const xml_node& RootNode, CEasyString& String);
	bool SaveToFile(const xml_node& RootNode, LPCTSTR FileName, int FileCodePage = -1, int FileChannel = FILE_CHANNEL_NORMAL1);
	///
	void set_null() { _buffer.Clear(); }
//	bool parse_file_in_mem(TCHAR *bits, size_t bytes, unsigned int optmsk = parse_noset)
//	bool parse_file_in_mem(TCHAR *bits, unsigned int optmsk = parse_noset)
//	{
//#ifdef PUGOPT_NONSEG
//		assert((optmsk & parse_wnorm) == 0); // Normalization isn't implemented for non-segmented strings, as of 24 Mar 2003
//#endif
//		clear(); //clear any existing data.
//		if(optmsk != parse_noset) _optmsk = optmsk;
//		{
//			_buffer = bits;
//			_xmldoc = pug::new_node(node_document);
//			_xmldoc->parent = _xmldoc; //Point to self.
//			TCHAR* s = parse( _buffer, _xmldoc, _growby, _optmsk );
//			_strpos = s;
//			return true;
//		}
//		return false;
//	}

#ifdef PUGOPT_MEMFIL

	//<summary>Parse the contents of the file at the given path, using a memory-mapped file.</summary>
	//<param name="path">File path.</param>
	//<param name="optmsk">Parser options.</param>
	//<returns>
	//	True (1) if the file was parsed successfully, false (0) if open failed,
	//	and -1 if an exception occured.
	//</returns>
	//<remarks>
	//	The file contents are available until closed by calling 'parse',
	//	'parse_file', 'clear' or '~xml_parser'.
	//</remarks>
	//int parse_mmfile(const TCHAR* path,unsigned int optmsk = parse_noset)
	//{
	//	int status = 0;
	//	if(path)
	//	{
	//		clear(); //Clear any existing data.
	//		if(optmsk != parse_noset) _optmsk = optmsk;
	//		assert((optmsk & parse_wnorm) == 0); //Normalization isn't implemented for memory-mapped files, as of 23 Jan 2003.
	//		const bool readonly = (optmsk & (parse_dtd|parse_dtd_only)) == 0;
	//		if(open_mmfile(path,readonly,false))
	//		{
	//			//If the file has a 0 at the end we are ok to proceed, otherwise add one.
	//			if  ( has_eos_terminator()
	//			   || open_mmfile(path,false,true) //Re-open and add 0 at EOF.
	//			    )
	//			{
	//				try
	//				{
	//					_xmldoc = new_node(node_document);
	//					_xmldoc->parent = _xmldoc; //Point to self.
	//					// TCHAR* s = pug::parse( (TCHAR*)_mmaddr, _xmldoc, _growby, _optmsk );
	//					TCHAR* s = parse( (TCHAR*)_mmaddr, _xmldoc, _growby, _optmsk );
	//					_strpos = s;
	//					status = 1;
	//				}
	//				catch(...)
	//				{
	//					status = -1;
	//					assert(false);
	//				}
	//			}
	//		}
	//	}
	//	return status;
	//}

protected:

	//<summary>Opens the specified memory-mapped file.</summary>
	//<param name="path">File path.</param>
	//<param name="readonly">True to open the file for read-only access.</param>
	//<param name="addeos">True to add a 0 to the end of the file.</param>
	//<returns>Success if the file was opened.</returns>
	//bool open_mmfile(const TCHAR* path,const bool readonly,const bool addeos)
	//{
	//	clear(); //Close any existing MMF and clear any existing data.
	//	assert(_mmfile == NULL && _mmfile == NULL && _mmaddr == NULL);
	//	_addeos = false;
	//	_mmfile = CreateFile(path,readonly?GENERIC_READ:GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL); //Open read-only, no share, no security attrs, ..., no template.
	//	if(_mmfile != INVALID_HANDLE_VALUE)
	//	{
	//		_mfsize = ::GetFileSize(_mmfile,NULL);
	//		_mmfmap = CreateFileMapping(_mmfile,NULL,readonly?PAGE_READONLY:PAGE_READWRITE,0,_mfsize+(addeos?sizeof(TCHAR):0),NULL); //Create map: handle, no security attr, read|read/write, larger if addeos, anonymous.
	//		if(_mmfmap != NULL)
	//		{
	//			assert(_mmaddr == NULL);
	//			_mmaddr = MapViewOfFile(_mmfmap,readonly?FILE_MAP_READ:FILE_MAP_WRITE,0,0,0); //Map the view: handle, read|read/write, start at beginning, map entire file.
	//			if(_mmaddr != NULL)
	//			{
	//				if(addeos) //Add a terminating 0 to the end of the file for 'parse()'.
	//				{
	//					assert(!readonly);
	//					*(((TCHAR*)_mmaddr) + _mfsize) = 0;
	//					_addeos = true;
	//				}
	//			}
	//			else
	//			{
	//				CloseHandle(_mmfmap);
	//				CloseHandle(_mmfile);
	//				_mmfile = _mmfmap = 0;
	//			}
	//		}
	//		else
	//		{
	//			CloseHandle(_mmfile);
	//			_mmfile = 0;
	//		}
	//	}
	//	return (_mmaddr != NULL);
	//}

    // returns true if there is a 0 at the end of the file.
    //<since>3 Mar 2003 NF</since>
    bool has_eos_terminator() const
    {
        try
        {       // if the file is a multiple of 4K (sector size?) an exception
                // can be raised here, hence the try - catch. NF added 3 Mar 2003
		    return ( *(((TCHAR*)_mmaddr) + _mfsize) == 0
				  || ( _mfsize > 0 && *(((TCHAR*)_mmaddr) + _mfsize - 1) == 0 )
				   );
        }
        catch( ... )
        {
            return false;
        }
    }

#endif

    // NF 29 May 2003
    TCHAR* parse( register TCHAR* s, xml_node_struct* xmldoc, int growby, unsigned int optmsk = parse_default );

};  // class xml_parser


//<summary>Static single-pass in-situ parse the given xml string.</summary>
//<param name="s">Pointer to XML-formatted string.</param>
//<param name="xmldoc">Pointer to root.</param>
//<param name="growby">Pointer space growth increment.</param>
//<param name="optmsk">Parse options mask.</param>
//<returns>Last string position or null.</returns>
//<remarks>
//	Input string is zero-segmented if 'PUGOPT_NONSEG' is not defined. Memory
//	may have been allocated to 'root' (free with 'free_node').
//</remarks>
// TCHAR* parse( register TCHAR* s, xml_node_struct* xmldoc, int growby, unsigned int optmsk = parse_default )
inline TCHAR* xml_parser::parse( register TCHAR* s, xml_node_struct* xmldoc, int growby, unsigned int optmsk /* = parse_default */ )
{
	if(!s || !xmldoc) return s;
	TCHAR ch = 0; //Current char, in cases where we must null-terminate before we test.
	xml_node_struct* cursor = xmldoc; //Tree node cursor.
	TCHAR* mark = s; //Marked string position for temporary look-ahead.
	while(*s!=0)
	{
LOC_SEARCH: //Obliviously search for next element.
		SCANFOR(chartype_enter(*s)); //Find the next '<'.
		if(chartype_enter(*s))
		{
			++s;
LOC_CLASSIFY: //What kind of element?
			if(chartype_pi(*s)) //'<?...'
			{
				++s;
				if(chartype_symbol(*s) && OPTSET(parse_pi))
				{
					mark = s;
					SCANFOR(chartype_pi(*s)); //Look for terminating '?'.
#ifndef PUGOPT_NONSEG
					if(chartype_pi(*s)) *s = _T('/'); //Same semantics as for '<.../>', so fudge it.
#endif
					s = mark;
					PUSHNODE(node_pi); //Append a new node on the tree.
					goto LOC_ELEMENT; //Go read the element name.
				}
				else //Bad PI or parse_pi not set.
				{
					SCANFOR(chartype_leave(*s)); //Look for '>'.
					++s;
					mark = 0;
					continue;
				}
			}
			else if(chartype_special(*s)) //'<!...'
			{
				++s;
				if(chartype_dash(*s)) //'<!-...'
				{
					++s;
					if(OPTSET(parse_comments) && chartype_dash(*s)) //'<!--...'
					{
						++s;
						PUSHNODE(node_comment); //Append a new node on the tree.
						cursor->value = s; //Save the offset.
						while(*s!=0 && *(s+1) && *(s+2) && !((chartype_dash(*s) && chartype_dash(*(s+1))) && chartype_leave(*(s+2)))) ++s; //Scan for terminating '-->'.
						if(*s==0) return s;
#ifdef PUGOPT_NONSEG
						SETLEN(); //NF 19 Jan 2003.
#else
						*s = 0; //Zero-terminate this segment at the first terminating '-'.
#endif
						if(OPTSET(parse_trim_comment)) //Trim whitespace.
						{
#ifdef PUGOPT_NONSEG
							strwtrim(&cursor->value,cursor->value_size);
#else
							if(OPTSET(parse_wnorm)) strwnorm(&cursor->value);
							else strwtrim(&cursor->value);
#endif
						}
						s += 2; //Step over the '\0-'.
						POPNODE(); //Pop since this is a standalone.
						goto LOC_LEAVE; //Look for any following PCDATA.
					}
					else
					{
						while(*s!=0 && *(s+1)!=0 && *(s+2)!=0 && !((chartype_dash(*s) && chartype_dash(*(s+1))) && chartype_leave(*(s+2)))) ++s; //Scan for terminating '-->'.
						if(*s==0) return s;
						s += 2;
						goto LOC_LEAVE; //Look for any following PCDATA.
					}
				}
				else if(chartype_lbracket(*s)) //'<![...'
				{
					++s;
					if(*s==_T('I')) //'<![I...'
					{
						++s;
						if(*s==_T('N')) //'<![IN...'
						{
							++s;
							if(*s==_T('C')) //'<![INC...'
							{
								++s;
								if(*s==_T('L')) //'<![INCL...'
								{
									++s;
									if(*s==_T('U')) //'<![INCLU...'
									{
										++s;
										if(*s==_T('D')) //'<![INCLUD...'
										{
											++s;
											if(*s==_T('E')) //'<![INCLUDE...'
											{
												++s;
												if(chartype_lbracket(*s)) //'<![INCLUDE[...'
												{
													++s;
													if(OPTSET(node_cdata))
													{
														PUSHNODE(node_include); //Append a new node on the tree.
														cursor->value = s; //Save the offset.
														while(!(chartype_rbracket(*s) && chartype_rbracket(*(s+1)) && chartype_leave(*(s+2)))) ++s; //Scan for terminating ']]>'.
														if(chartype_rbracket(*s))
														{
#ifdef PUGOPT_NONSEG
															SETLEN(); //NF 19 Jan 2003.
#else
															*s = 0; //Zero-terminate this segment.
#endif
															++s;
															if(OPTSET(parse_trim_cdata)) //Trim whitespace.
															{
#ifdef PUGOPT_NONSEG
																strwtrim(&cursor->value, cursor->value_size);
#else
																if(OPTSET(parse_wnorm)) strwnorm(&cursor->value);
																else strwtrim(&cursor->value);
#endif
															}
														}
														POPNODE(); //Pop since this is a standalone.
													}
													else //Flagged for discard, but we still have to scan for the terminator.
													{
														while(*s!=0 && *(s+1)!=0 && *(s+2)!=0 && !(chartype_rbracket(*s) && chartype_rbracket(*(s+1)) && chartype_leave(*(s+2)))) ++s; //Scan for terminating ']]>'.
														++s;
													}
													++s; //Step over the last ']'.
													goto LOC_LEAVE; //Look for any following PCDATA.
												}
											}
										}
									}
								}
							}
						}
					}
					else if(*s==_T('C')) //'<![C...'
					{
						++s;
						if(*s==_T('D')) //'<![CD...'
						{
							++s;
							if(*s==_T('A')) //'<![CDA...'
							{
								++s;
								if(*s==_T('T')) //'<![CDAT...'
								{
									++s;
									if(*s==_T('A')) //'<![CDATA...'
									{
										++s;
										if(chartype_lbracket(*s)) //'<![CDATA[...'
										{
											++s;
											if(OPTSET(parse_cdata))
											{
												PUSHNODE(node_cdata); //Append a new node on the tree.
												cursor->value = s; //Save the offset.
												while(*s!=0 && *(s+1)!=0 && *(s+2)!=0 && !(chartype_rbracket(*s) && chartype_rbracket(*(s+1)) && chartype_leave(*(s+2)))) ++s; //Scan for terminating ']]>'.
												if(*(s+2)==0) return s; //Very badly formed.
												if(chartype_rbracket(*s))
												{
#ifdef PUGOPT_NONSEG
													SETLEN(); //NF 19 Jan 2003.
#else
													*s = 0; //Zero-terminate this segment.
#endif
													++s;
													if(OPTSET(parse_trim_cdata)) //Trim whitespace.
													{
#ifdef PUGOPT_NONSEG
														strwtrim(&cursor->value,cursor->value_size);
#else
														if(OPTSET(parse_wnorm)) strwnorm(&cursor->value);
														else strwtrim(&cursor->value);
#endif
													}
												}
												POPNODE(); //Pop since this is a standalone.
											}
											else //Flagged for discard, but we still have to scan for the terminator.
											{
												while(*s!=0 && *(s+1)!=0 && *(s+2)!=0 && !(chartype_rbracket(*s) && chartype_rbracket(*(s+1)) && chartype_leave(*(s+2)))) ++s; //Scan for terminating ']]>'.
												++s;
											}
											++s; //Step over the last ']'.
											goto LOC_LEAVE; //Look for any following PCDATA.
										}
									}
								}
							}
						}
					}
					continue; //Probably a corrupted CDATA section, so just eat it.
				}
				else if(*s==_T('D')) //'<!D...'
				{
					++s;
					if(*s==_T('O')) //'<!DO...'
					{
						++s;
						if(*s==_T('C')) //'<!DOC...'
						{
							++s;
							if(*s==_T('T')) //'<!DOCT...'
							{
								++s;
								if(*s==_T('Y')) //'<!DOCTY...'
								{
									++s;
									if(*s==_T('P')) //'<!DOCTYP...'
									{
										++s;
										if(*s==_T('E')) //'<!DOCTYPE...'
										{
											++s;
											SKIPWS(); //Eat any whitespace.
											xml_attribute_struct* a = 0;
											if(OPTSET(parse_doctype))
											{
												PUSHNODE(node_doctype); //Append a new node on the tree.
												a = append_attribute(cursor,3); //Store the DOCTYPE name.
												a->value = a->name = s; //Save the offset.
                                                // Why are value & name set to the same thing here and below. NF 25 Mar 2003
                                                // Also why not store this in node.name. See my CP post. NF 25 Mar 2003
											}
											SCANWHILE(chartype_symbol(*s)); //'<!DOCTYPE symbol...'
#ifdef PUGOPT_NONSEG
											if(OPTSET(parse_doctype))
												a->name_size = a->value_size = s - a->value; //Save the length. rem: Before ENDSEG()
#endif
											ENDSEG(); //Save char in 'ch', terminate & step over.
											if(chartype_space(ch)) SKIPWS(); //Eat any whitespace.
LOC_DOCTYPE_SYMBOL:
											if(chartype_symbol(*s))
											{
												mark = s;
												SCANWHILE(chartype_symbol(*s)); //'...symbol SYSTEM...'
												if(OPTSET(parse_doctype))
												{
													a = append_attribute(cursor,1);
													a->value = a->name = mark;
#ifdef PUGOPT_NONSEG
													a->value_size = a->name_size = s - mark; //NF 19 Jan 2003.
#else
													*s = 0;
#endif
												}
												++s;
												SKIPWS();
											}
											if(chartype_quote(*s)) //'...SYSTEM "..."'
											{
LOC_DOCTYPE_QUOTE:
												ch = *s;
												++s;
												mark = s;
												while(*s!=0 && *s != ch) ++s;
												if(*s!=0)
												{
													if(OPTSET(parse_doctype))
													{
														a = append_attribute(cursor,1);
														a->value = mark;
#ifdef PUGOPT_NONSEG
														a->value_size = s - mark; //NF 19 Jan 2003.
#else
														*s = 0;
#endif
													}
													++s;
													SKIPWS(); //Eat whitespace.
													if(chartype_quote(*s)) goto LOC_DOCTYPE_QUOTE; //Another quoted section to store.
													else if(chartype_symbol(*s)) goto LOC_DOCTYPE_SYMBOL; //Not wellformed, but just parse it.
												}
											}
											if(chartype_lbracket(*s)) //'...[...'
											{
												++s; //Step over the bracket.
												if(OPTSET(parse_doctype)) cursor->value = s; //Store the offset.
												unsigned int bd = 1; //Bracket depth counter.
												while(*s!=0) //Loop till we're out of all brackets.
												{
													if(chartype_rbracket(*s)) --bd;
													else if(chartype_lbracket(*s)) ++bd;
													if(bd == 0) break;
													++s;
												}
												//Note: 's' now points to end of DTD, i.e.: ']'.
												if(OPTSET(parse_doctype))
												{
													//Note: If we aren't parsing the DTD ('!parse_dtd', etc.) then it is stored in the DOM as one whole chunk.
#ifdef PUGOPT_NONSEG
													SETLEN(); //NF 19 Jan 2003
#else
													*s = 0; //Zero-terminate.
#endif
													if(OPTSET(parse_dtd)||OPTSET(parse_dtd_only))
													{
														if(OPTSET(parse_dtd))
														{
#ifdef PUGOPT_NONSEG
															TCHAR svch = *s;
															try
															{
																*s = 0; //Zero-terminate.
																parse( cursor->value, cursor, growby, optmsk ); //Parse it.
                                                                // cursor->value will contain the entire DTD.
                                                                // probably better to set it to empty. See cmt below. NF 25 Mar 2003
															}
															catch(...){ assert(false); }
															*s = svch;
#else
                                                            // this will change cursor->value. we'll finish up with
                                                            // a 0 after the first DTD item. I can't see any point
                                                            // in having cursor->value set after parse(). See cmt above & my CP post. NF 25 Mar 2003
															parse( cursor->value, cursor, growby, optmsk ); //Parse it.
#endif
														}
														if(OPTSET(parse_dtd_only)) return (s+1); //Flagged to parse DTD only, so leave here.
													}
													else if(OPTSET(parse_trim_doctype)) //Trim whitespace.
													{
#ifdef PUGOPT_NONSEG
														strwtrim(&cursor->value, cursor->value_size);
#else
														if(OPTSET(parse_wnorm)) strwnorm(&cursor->value);
														else strwtrim(&cursor->value);
#endif
													}
													++s; //Step over the zero.
													POPNODE(); //Pop since this is a standalone.
												}
												SCANFOR(chartype_leave(*s));
												continue;
											}
											//Fall-through; make sure we pop.
											POPNODE(); //Pop since this is a standalone.
											continue;
										}
									}
								}
							}
						}
					}
				}
				else if(chartype_symbol(*s)) //An inline DTD tag.
				{
					mark = s;
					SCANWHILE(chartype_symbol(*s));
					ENDSEG(); //Save char in 'ch', terminate & step over.
					xml_node_type e = node_dtd_entity;
#ifdef PUGOPT_NONSEG
					const unsigned int dtdilen = (s - 1) - mark;
					if(_tcsncmp(mark,_T("ATTLIST"),Max((7*sizeof(TCHAR)),dtdilen))==0) e = node_dtd_attlist;
					else if(_tcsncmp(mark,_T("ELEMENT"),Max((7*sizeof(TCHAR)),dtdilen))==0) e = node_dtd_element;
					else if(_tcsncmp(mark,_T("NOTATION"),Max((8*sizeof(TCHAR)),dtdilen))==0) e = node_dtd_notation;
#else
					if(_tcscmp(mark,_T("ATTLIST"))==0) e = node_dtd_attlist;
					else if(_tcscmp(mark,_T("ELEMENT"))==0) e = node_dtd_element;
					else if(_tcscmp(mark,_T("NOTATION"))==0) e = node_dtd_notation;
#endif
					PUSHNODE(e); //Append a new node on the tree.
					if(*s!=0 && chartype_space(ch))
					{
						SKIPWS(); //Eat whitespace.
						if(chartype_symbol(*s) || *s==_T('%'))
						{
							mark = s;
							if(*s==_T('%')) //Could be '<!ENTITY % name' -or- '<!ENTITY %name'
							{
#ifdef PUGOPT_NONSEG
								//Note: For memory-mapped file support we need to treat 's' as read-only so we can't do '*(s-1) = _T('%');' below.
								cursor->name = mark; //Sort out extraneous whitespace when we retrieve it. TODO: Whitespace cleanup.
#endif
								++s;
								if(chartype_space(*s))
								{
									SKIPWS(); //Eat whitespace.
#ifndef PUGOPT_NONSEG
									*(s-1) = _T('%');
									cursor->name = (s-1);
#endif
								}
#ifndef PUGOPT_NONSEG
								else cursor->name = mark;
#endif
							}
							else cursor->name = s;
							SCANWHILE(chartype_symbol(*s));
#ifdef PUGOPT_NONSEG
							cursor->name_size = s - cursor->name;
#endif
							ENDSEG(); //Save char in 'ch', terminate & step over.
							if(chartype_space(ch))
							{
								SKIPWS(); //Eat whitespace.
								if(e == node_dtd_entity) //Special case; may have multiple quoted sections w/anything inside.
								{
									cursor->value = s; //Just store everything here.
									bool qq = false; //Quote in/out flag.
									while(*s != 0) //Loop till we find the right sequence.
									{
										if(!qq && chartype_quote(*s)){ ch = *s; qq = true; }
										else if(qq && *s == ch) qq = false;
										else if(!qq && chartype_leave(*s)) //Not in quoted reqion and '>' hit.
										{
#ifdef PUGOPT_NONSEG
											SETLEN(); //NF 19 Jan 2003.
#else
											*s = 0;
#endif
											++s;
											if(OPTSET(parse_trim_entity))
											{
#ifdef PUGOPT_NONSEG
												strwtrim(&cursor->value,cursor->value_size);
#else
												if(OPTSET(parse_wnorm)) strwnorm(&cursor->value);
												else strwtrim(&cursor->value);
#endif
											}
											POPNODE();
											goto LOC_SEARCH;
										}
										++s;
									}
									if(OPTSET(parse_trim_entity))
									{
#ifdef PUGOPT_NONSEG
										strwtrim(&cursor->value, cursor->value_size);
#else
										if(OPTSET(parse_wnorm)) strwnorm(&cursor->value);
										else strwtrim(&cursor->value);
#endif
									}
								}
								else
								{
									cursor->value = s;
									SCANFOR(chartype_leave(*s)); //Just look for '>'.
#ifdef PUGOPT_NONSEG
									SETLEN(); //NF 19 Jan 2003.
#else
									*s = 0;
#endif
									++s;
									if(OPTSET(parse_trim_entity))
									{
#ifdef PUGOPT_NONSEG
										strwtrim(&cursor->value, cursor->value_size);
#else
										if(OPTSET(parse_wnorm)) strwnorm(&cursor->value);
										else strwtrim(&cursor->value);
#endif
									}
									POPNODE();
									goto LOC_SEARCH;
								}
							}
						}
					}
					POPNODE();
				}
			}
			else if(chartype_symbol(*s)) //'<#...'
			{
				cursor = append_node(cursor,growby); //Append a new node to the tree.
LOC_ELEMENT: //Scan for & store element name.
				cursor->name = s;
				SCANWHILE(chartype_symbol(*s)); //Scan for a terminator.
#ifdef PUGOPT_NONSEG
				cursor->name_size = s - cursor->name; //Note: Before ENDSEG().
#endif
				ENDSEG(); //Save char in 'ch', terminate & step over.
				if
					(
					*s!=0 &&
					(
					chartype_close(ch) //'</...'
#ifdef PUGOPT_NONSEG
					//||
					//chartype_pi(ch) //Treat '?>' as '/>' NF 19 Jan 2003
#endif
					)
					)
				{
					SCANFOR(chartype_leave(*s)); //Scan for '>', stepping over the tag name.
					POPNODE(); //Pop.
					continue;
				}
				else if(*s!=0 && !chartype_space(ch)) goto LOC_PCDATA; //No attributes, so scan for PCDATA.
				else if(*s!=0 && chartype_space(ch))
				{
					SKIPWS(); //Eat any whitespace.
LOC_ATTRIBUTE:
					if(chartype_symbol(*s)) //<... #...
					{
						xml_attribute_struct* a = append_attribute(cursor,growby); //Make space for this attribute.
						a->name = s; //Save the offset.
						SCANWHILE(chartype_symbol(*s)); //Scan for a terminator.
#ifdef PUGOPT_NONSEG
						ENDSEGNAM(a);
#else
						ENDSEG(); //Save char in 'ch', terminate & step over.
#endif
						if(*s!=0 && chartype_space(ch)) SKIPWS(); //Eat any whitespace.
						if(*s!=0 && (chartype_equals(ch) || chartype_equals(*s))) //'<... #=...'
						{
							if(chartype_equals(*s)) ++s;
							SKIPWS(); //Eat any whitespace.
							if(chartype_quote(*s)) //'<... #="...'
							{
								ch = *s; //Save quote char to avoid breaking on "''" -or- '""'.
								++s; //Step over the quote.
								a->value = s; //Save the offset.
								SCANFOR(*s == ch); //Scan for the terminating quote, or '>'.
#ifdef PUGOPT_NONSEG
								ENDSEGATT(a);
#else
								ENDSEG(); //Save char in 'ch', terminate & step over.
#endif
								if(OPTSET(parse_trim_attribute)) //Trim whitespace.
								{
#ifdef PUGOPT_NONSEG
									strwtrim(&a->value,a->value_size);
#else
									if(OPTSET(parse_wnorm)) strwnorm(&a->value);
									else strwtrim(&a->value);
#endif
								}

#ifdef PUGOPT_NODE_FLAGS        // NF 29 May 2003
                                // If we have an "expanded=true" attribute set expand().
                                if ( _tcsncmp( a->name, _T("expanded"), Max( (8*sizeof(TCHAR)), a->name_size ) ) == 0
                                  && _tcsncmp( a->value, _T("true"), Max( 4*sizeof(TCHAR), a->value_size ) ) == 0
                                   )
                                {
                                    cursor->expand( true );
                                    _expanded_nodes.push_back( cursor );
                                }
#endif

								if(chartype_leave(*s)){ ++s; goto LOC_PCDATA; }
								else if(chartype_close(*s))
								{
									++s;
									POPNODE();
									SKIPWS(); //Eat any whitespace.
									if(chartype_leave(*s)) ++s;
									goto LOC_PCDATA;
								}
								if(chartype_space(*s)) //This may indicate a following attribute.
								{
									SKIPWS(); //Eat any whitespace.
									goto LOC_ATTRIBUTE; //Go scan for additional attributes.
								}
							}
						}
						if(chartype_symbol(*s)) goto LOC_ATTRIBUTE;
						else if(*s!=0 && cursor->type == node_pi)
						{
#ifdef PUGOPT_NONSEG
							SCANFOR(chartype_pi(*s)); //compliments change where we don't fudge to '/>' when we find the PI. NF 20 Jan 2003
							SKIPWS(); //Eat any whitespace.
							if(chartype_pi(*s)) ++s;
#else
							SCANFOR(chartype_close(*s));
							SKIPWS(); //Eat any whitespace.
							if(chartype_close(*s)) ++s;
#endif
							SKIPWS(); //Eat any whitespace.
							if(chartype_leave(*s)) ++s;
							POPNODE();
							goto LOC_PCDATA;
						}
					}
				}
LOC_LEAVE:
				if(chartype_leave(*s)) //'...>'
				{
					++s; //Step over the '>'.
LOC_PCDATA: //'>...<'
					mark = s; //Save this offset while searching for a terminator.
					SKIPWS(); //Eat whitespace if no genuine PCDATA here.
					if(chartype_enter(*s)) //We hit a '<...', with only whitespace, so don't bother storing anything.
					{
						if(chartype_close(*(s+1))) //'</...'
						{
							SCANFOR(chartype_leave(*s)); //Scan for '>', stepping over any end-tag name.
							POPNODE(); //Pop.
							continue; //Continue scanning.
						}
						else goto LOC_SEARCH; //Expect a new element enter, so go scan for it.
					}
					s = mark; //We hit something other than whitespace; restore the original offset.
					PUSHNODE(node_pcdata); //Append a new node on the tree.
					cursor->value = s; //Save the offset.
					SCANFOR(chartype_enter(*s)); //'...<'
#ifdef PUGOPT_NONSEG
					ENDSEGDAT();
#else
					ENDSEG(); //Save char in 'ch', terminate & step over.
#endif
					if(OPTSET(parse_trim_pcdata)) //Trim whitespace.
					{
#ifdef PUGOPT_NONSEG
						strwtrim(&cursor->value,cursor->value_size);
#else
						if(OPTSET(parse_wnorm)) strwnorm(&cursor->value);
						else strwtrim(&cursor->value);
#endif
					}
					POPNODE(); //Pop since this is a standalone.
					if(chartype_enter(ch)) //Did we hit a '<...'?
					{
						if(chartype_close(*s)) //'</...'
						{
							SCANFOR(chartype_leave(*s)); //'...>'
							POPNODE(); //Pop.
							goto LOC_LEAVE;
						}
						else if(chartype_special(*s)) goto LOC_CLASSIFY; //We hit a '<!...'. We must test this here if we want comments intermixed w/PCDATA.
						else if(*s) goto LOC_CLASSIFY;
						else return s;
					}
				}
				//Fall-through A.
				else if(chartype_close(*s)) //'.../'
				{
					++s;
					if(chartype_leave(*s)) //'.../>'
					{
						POPNODE(); //Pop.
						++s;
						continue;
					}
				}
			}
			//Fall-through B.
			else if(chartype_close(*s)) //'.../'
			{
				SCANFOR(chartype_leave(*s)); //'.../>'
				POPNODE(); //Pop.
				continue;
			}
		}
	}
	return s;
}  // xml_parser::parse()



//<summary>An array of nodes, used by xml_node queries.</summary>
class xml_node_list: public pointer_array
{
public:
	xml_node_list(unsigned int grow = 4) : pointer_array(grow) { }
	virtual ~xml_node_list(){ }
public:
	xml_node at(int i){ return xml_node((xml_node_struct*)pointer_array::at((unsigned int)i)); } //Access xml_node at subscript.
	xml_node operator[](int i){ return xml_node((xml_node_struct*)pointer_array::at((unsigned int)i)); } //Access xml_node at subscript.
	friend std::ostream& operator<<(std::ostream& os,xml_node_list& list) //Output helper.
	{
		if(!os.good()) return os;
		unsigned int n = list.size();
		for(unsigned int i=0; i<n; ++i) os << list[i];
		return os;
	}
};


}; //namespace pug


//<summary>Check for Pug XML Library Variant/API Version campatibility.</summary>
#if !defined(PUGAPI_VARIANT) || (PUGAPI_VARIANT != PUGAPI_INTERNAL_VARIANT)
#	pragma message("Severe: The Pug XML library API variant does not match your implementation!")
#	pragma message("Severe: Your implementation may not run correctly with this variant.")
#	pragma message("Note: Read pugxml.xml for transition instructions.")
#	ifndef PUGAPI_VARIANT
#		pragma message("Note: Add #define PUGAPI_VARIANT X in your implementation, where X is:")
#	else //PUGAPI_VARIANT
#		pragma message("Note: Update #define PUGAPI_VARIANT X in your implementation, where X is:")
#	endif //!PUGAPI_VARIANT
#	pragma message("pugxml.h(39) : Value of PUGAPI_INTERNAL_VARIANT.")
#else //defined(PUGAPI_VARIANT)
#	if !defined(PUGAPI_VERSION_MAJOR) || (PUGAPI_VERSION_MAJOR != PUGAPI_INTERNAL_VERSION_MAJOR)
#		pragma message("Warning: The Pug XML library API major version does not match your implementation.")
#		pragma message("Warning: Your implementation may not run correctly with this major version.")
#		pragma message("Note: Read pugxml.xml for transition instructions.")
#		ifndef PUGAPI_VERSION_MAJOR
#			pragma message("Note: Add #define PUGAPI_VERSION_MAJOR X in your implementation, where X is:")
#		else //PUGAPI_VERSION_MAJOR
#			pragma message("Note: Update #define PUGAPI_VERSION_MAJOR X in your implementation, where X is:")
#		endif //!PUGAPI_VERSION_MAJOR
#		pragma message("pugxml.h(44) : Value of PUGAPI_INTERNAL_VERSION_MAJOR.")
#	else //defined(PUGAPI_VERSION_MAJOR)
#		if !defined(PUGAPI_VERSION_MINOR) || (PUGAPI_VERSION_MINOR != PUGAPI_INTERNAL_VERSION_MINOR)
#			pragma message("Note: The Pug XML library API minor version does not match your implementation.")
#			pragma message("Note: Your implementation may not run correctly with this minor version.")
#			pragma message("Note: Read pugxml.xml for transition instructions.")
#			ifndef PUGAPI_VERSION_MINOR
#				pragma message("Note: Add #define PUGAPI_VERSION_MINOR X in your implementation, where X is:")
#			else //PUGAPI_VERSION_MINOR
#				pragma message("Note: Update #define PUGAPI_VERSION_MINOR X in your implementation, where X is:")
#			endif //!PUGAPI_VERSION_MINOR
#			pragma message("pugxml.h(45) : Value of PUGAPI_INTERNAL_VERSION_MINOR.")
#		endif //!defined(PUGAPI_VERSION_MINOR)
#	endif //!defined(PUGAPI_VERSION_MAJOR)
#endif //!defined(PUGAPI_VARIANT)


#endif //!defined(_PUGXML_)

