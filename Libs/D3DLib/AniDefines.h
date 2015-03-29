#pragma once

namespace D3DLib{


enum SST_ANIMATION_BLOCK
{
	SST_AB_INTERPOLATION_TYPE=1,
	SST_AB_GLOBALSEQUENCE_ID,
	SST_AB_ANIMATION_FRAME,		
};
enum SST_ANIMATION_FRAME
{
	SST_AF_TIMESTAMPS=1,
	SST_AF_KEYS,
};

enum INTERPOLATE_TYPE
{
	INTERPOLATE_TYPE_NONE,
	INTERPOLATE_TYPE_LINEAR,
	INTERPOLATE_TYPE_HERMITE,
};

template<class KEY_TYPE>
struct ANIMATION_FRAME
{		
	CEasyArray<UINT>			TimeStamps;
	CEasyArray<KEY_TYPE>		Keys;
};
template<class KEY_TYPE>
struct FAKE_ANIMATION_FRAME
{		
	CEasyArray<FLOAT>			TimeStamps;
	CEasyArray<KEY_TYPE>		Keys;
	void Clear()
	{
		TimeStamps.Clear();
		Keys.Clear();
	}
};
template<class KEY_TYPE>
struct ANIMATION_BLOCK
{
	int										InterpolationType;
	int										GlobalSequenceID;
	CEasyArray<ANIMATION_FRAME<KEY_TYPE> >	Animations;
	void Clear()
	{
		InterpolationType=INTERPOLATE_TYPE_NONE;
		GlobalSequenceID=-1;
		Animations.Clear();
	}
};
typedef ANIMATION_BLOCK<CD3DVector3> ANIMATION_TRANSLATION_BLOCK;
typedef ANIMATION_BLOCK<CD3DQuaternion> ANIMATION_ROTATION_BLOCK;
typedef ANIMATION_BLOCK<CD3DVector3> ANIMATION_SCALING_BLOCK;
typedef ANIMATION_BLOCK<CD3DVector3> ANIMATION_COLOR_BLOCK;
typedef ANIMATION_BLOCK<FLOAT> ANIMATION_ALPHA_BLOCK;


//bool GetInterpolationValue(UINT Time,bool IsLoop,int InterpolationType,UINT AniLength,ANIMATION_FRAME<FLOAT>& AniFrame,FLOAT& Value);
//bool GetInterpolationValue(UINT Time,bool IsLoop,int InterpolationType,UINT AniLength,ANIMATION_FRAME<CD3DVector3>& AniFrame,CD3DVector3& Value);
//bool GetInterpolationValue(UINT Time,bool IsLoop,int InterpolationType,UINT AniLength,ANIMATION_FRAME<CD3DQuaternion>& AniFrame,CD3DQuaternion& Value);
inline bool GetInterpolationValue(UINT Time,bool IsLoop,int InterpolationType,UINT AniLength,ANIMATION_FRAME<FLOAT>& AniFrame,FLOAT& Value)
{
	UINT FrameCount=AniFrame.TimeStamps.GetCount();	
	bool ValueChanged=false;

	if(FrameCount==1)
	{			
		Value=AniFrame.Keys[0];
		ValueChanged=true;
	}
	else if(FrameCount)
	{				
		UINT FrameTime=Time;
		UINT Frame=0;
		if(IsLoop)
		{
			if(AniLength>0)
				FrameTime=FrameTime%AniLength;	
			else if(FrameCount)
				FrameTime=FrameTime%AniFrame.TimeStamps[FrameCount-1];
		}
		else
		{
			if(AniLength>0)
			{
				if(FrameTime>AniLength)
					FrameTime=AniLength;
			}
			else if(FrameCount)
			{
				if(FrameTime>AniFrame.TimeStamps[FrameCount-1])
					FrameTime=AniFrame.TimeStamps[FrameCount-1];
			}
		}		
		for(;Frame<AniFrame.TimeStamps.GetCount()-1;Frame++)
		{
			if(FrameTime>=AniFrame.TimeStamps[Frame]&&
				FrameTime<AniFrame.TimeStamps[Frame+1])
				break;
		}
		if(Frame<AniFrame.TimeStamps.GetCount()-1)
		{
			FLOAT S=(FLOAT)(FrameTime-AniFrame.TimeStamps[Frame])/
				(FLOAT)(AniFrame.TimeStamps[Frame+1]-AniFrame.TimeStamps[Frame]);
			switch(InterpolationType)
			{
			case INTERPOLATE_TYPE_NONE:
				Value=AniFrame.Keys[Frame];							
				break;
			case INTERPOLATE_TYPE_LINEAR:
				Value=AniFrame.Keys[Frame]*(1.0f-S)+AniFrame.Keys[Frame+1]*S;
				break;
			case INTERPOLATE_TYPE_HERMITE:
				PrintSystemLog(0,_T("Unsupport Hermite Interpolate in GetInterpolationValue"));
				break;
			}				
		}
		else
		{
			Value=AniFrame.Keys[Frame];
		}
		ValueChanged=true;
	}
	return ValueChanged;
}

inline bool GetInterpolationValue(UINT Time,bool IsLoop,int InterpolationType,UINT AniLength,ANIMATION_FRAME<CD3DVector3>& AniFrame,CD3DVector3& Value)
{
	UINT FrameCount=AniFrame.TimeStamps.GetCount();
	bool ValueChanged=false;

	if(FrameCount==1)
	{			
		Value=AniFrame.Keys[0];
		ValueChanged=true;
	}
	else if(FrameCount)
	{				
		UINT FrameTime=Time;
		UINT Frame=0;
		if(IsLoop)
		{
			if(AniLength>0)
				FrameTime=FrameTime%AniLength;	
			else if(FrameCount)
				FrameTime=FrameTime%AniFrame.TimeStamps[FrameCount-1];
		}
		else
		{
			if(AniLength>0)
			{
				if(FrameTime>AniLength)
					FrameTime=AniLength;
			}
			else if(FrameCount)
			{
				if(FrameTime>AniFrame.TimeStamps[FrameCount-1])
					FrameTime=AniFrame.TimeStamps[FrameCount-1];
			}
		}		
		for(;Frame<AniFrame.TimeStamps.GetCount()-1;Frame++)
		{
			if(FrameTime>=AniFrame.TimeStamps[Frame]&&
				FrameTime<AniFrame.TimeStamps[Frame+1])
				break;
		}
		if(Frame<AniFrame.TimeStamps.GetCount()-1)
		{
			FLOAT S=(FLOAT)(FrameTime-AniFrame.TimeStamps[Frame])/
				(FLOAT)(AniFrame.TimeStamps[Frame+1]-AniFrame.TimeStamps[Frame]);
			switch(InterpolationType)
			{
			case INTERPOLATE_TYPE_NONE:
				Value=AniFrame.Keys[Frame];							
				break;
			case INTERPOLATE_TYPE_LINEAR:
				Value=CD3DVector3::Lerp(AniFrame.Keys[Frame],AniFrame.Keys[Frame+1],S);
				break;
			case INTERPOLATE_TYPE_HERMITE:
				PrintSystemLog(0,_T("Unsupport Hermite Interpolate in GetInterpolationValue"));
				break;
			}				
		}
		else
		{
			Value=AniFrame.Keys[Frame];
		}	
		ValueChanged=true;
	}
	return ValueChanged;
}

inline bool GetInterpolationValue(UINT Time,bool IsLoop,int InterpolationType,UINT AniLength,ANIMATION_FRAME<CD3DQuaternion>& AniFrame,CD3DQuaternion& Value)
{
	UINT FrameCount=AniFrame.TimeStamps.GetCount();
	bool ValueChanged=false;

	Value.SetIdentity();

	if(FrameCount==1)
	{			
		Value=AniFrame.Keys[0];
		ValueChanged=true;
	}
	else if(FrameCount)
	{				
		UINT FrameTime=Time;
		UINT Frame=0;
		if(IsLoop)
		{
			if(AniLength>0)
				FrameTime=FrameTime%AniLength;	
			else if(FrameCount)
				FrameTime=FrameTime%AniFrame.TimeStamps[FrameCount-1];
		}
		else
		{
			if(AniLength>0)
			{
				if(FrameTime>AniLength)
					FrameTime=AniLength;
			}
			else if(FrameCount)
			{
				if(FrameTime>AniFrame.TimeStamps[FrameCount-1])
					FrameTime=AniFrame.TimeStamps[FrameCount-1];
			}
		}		
		for(;Frame<AniFrame.TimeStamps.GetCount()-1;Frame++)
		{
			if(FrameTime>=AniFrame.TimeStamps[Frame]&&
				FrameTime<AniFrame.TimeStamps[Frame+1])
				break;
		}
		if(Frame<AniFrame.TimeStamps.GetCount()-1)
		{
			FLOAT S=(FLOAT)(FrameTime-AniFrame.TimeStamps[Frame])/
				(FLOAT)(AniFrame.TimeStamps[Frame+1]-AniFrame.TimeStamps[Frame]);
			switch(InterpolationType)
			{
			case INTERPOLATE_TYPE_NONE:
				Value=AniFrame.Keys[Frame];							
				break;
			case INTERPOLATE_TYPE_LINEAR:
				Value=CD3DQuaternion::SLerp(AniFrame.Keys[Frame],AniFrame.Keys[Frame+1],S);
				break;
			case INTERPOLATE_TYPE_HERMITE:
				PrintSystemLog(0,_T("Unsupport Hermite Interpolate in GetInterpolationValue"));
				break;
			}				
		}
		else
		{
			Value=AniFrame.Keys[Frame];
		}	
		ValueChanged=true;
	}
	return ValueChanged;
}

//bool GetInterpolationTransform(UINT Time,bool IsLoop,UINT AniIndex,UINT AniLength,CD3DMatrix& TransformMatrix,ANIMATION_BLOCK<CD3DVector3>& Translations,ANIMATION_BLOCK<CD3DQuaternion>& Rotations,ANIMATION_BLOCK<CD3DVector3>& Scalings);
//bool GetInterpolationTransformGlobal(UINT Time,bool IsLoop,CD3DMatrix& TransformMatrix,ANIMATION_BLOCK<CD3DVector3>& Translations,ANIMATION_BLOCK<CD3DQuaternion>& Rotations,ANIMATION_BLOCK<CD3DVector3>& Scalings,bool ForceGlobalSequence);
//bool GetInterpolationTransformWithGlobal(UINT Time,bool IsLoop,UINT AniIndex,UINT AniLength,CD3DMatrix& TransformMatrix,ANIMATION_BLOCK<CD3DVector3>& Translations,ANIMATION_BLOCK<CD3DQuaternion>& Rotations,ANIMATION_BLOCK<CD3DVector3>& Scalings);

inline bool GetInterpolationTransform(UINT Time,bool IsLoop,UINT AniIndex,UINT AniLength,CD3DMatrix& TransformMatrix,ANIMATION_BLOCK<CD3DVector3>& Translations,ANIMATION_BLOCK<CD3DQuaternion>& Rotations,ANIMATION_BLOCK<CD3DVector3>& Scalings)
{

	bool IsAnimated=false;

	CD3DVector3 Translation(0,0,0);
	CD3DQuaternion Rotation;
	CD3DVector3 Scaling(1.0f,1.0f,1.0f);
	Rotation.SetIdentity();


	if(AniIndex<Translations.Animations.GetCount())
	{
		GetInterpolationValue(Time,IsLoop,
			Translations.InterpolationType,
			AniLength,
			Translations.Animations[AniIndex],
			Translation);
		IsAnimated=true;
	}

	if(AniIndex<Rotations.Animations.GetCount())
	{	
		GetInterpolationValue(Time,IsLoop,
			Rotations.InterpolationType,
			AniLength,
			Rotations.Animations[AniIndex],
			Rotation);
		//Rotation.Normalize();
		IsAnimated=true;
	}

	if(AniIndex<Scalings.Animations.GetCount())
	{
		GetInterpolationValue(Time,IsLoop,
			Scalings.InterpolationType,
			AniLength,
			Scalings.Animations[AniIndex],
			Scaling);
		IsAnimated=true;
	}

	if(IsAnimated)
		//TransformMatrix=CD3DMatrix::FromScale(Scaling)*CD3DMatrix::FromRotationQuaternion(Rotation)*CD3DMatrix::FromTranslation(Translation);
		TransformMatrix.SetTransformation(&Scaling,&Rotation,&Translation);


	return IsAnimated;

}

inline bool GetInterpolationTransformGlobal(UINT Time,bool IsLoop,CD3DMatrix& TransformMatrix,ANIMATION_BLOCK<CD3DVector3>& Translations,ANIMATION_BLOCK<CD3DQuaternion>& Rotations,ANIMATION_BLOCK<CD3DVector3>& Scalings,CEasyArray<UINT>& GlobalSequences,bool ForceGlobalSequence)
{

	bool IsAnimated=false;

	CD3DVector3 Translation(0,0,0);
	CD3DQuaternion Rotation;
	CD3DVector3 Scaling(1.0f,1.0f,1.0f);
	Rotation.SetIdentity();

	if(Translations.GlobalSequenceID>=0&&Translations.GlobalSequenceID<(int)GlobalSequences.GetCount())
	{
		if(Translations.Animations.GetCount())
		{
			GetInterpolationValue(Time,IsLoop,
				Translations.InterpolationType,
				GlobalSequences[Translations.GlobalSequenceID],
				Translations.Animations[0],
				Translation);
			IsAnimated=true;
		}			
	}
	else if(ForceGlobalSequence)
	{
		if(Translations.Animations.GetCount())
		{

			GetInterpolationValue(Time,IsLoop,
				Translations.InterpolationType,
				0,
				Translations.Animations[0],
				Translation);
			IsAnimated=true;
		}
	}

	if(Rotations.GlobalSequenceID>=0&&Rotations.GlobalSequenceID<(int)GlobalSequences.GetCount())
	{		
		if(Rotations.Animations.GetCount())
		{	
			GetInterpolationValue(Time,IsLoop,
				Rotations.InterpolationType,
				GlobalSequences[Rotations.GlobalSequenceID],
				Rotations.Animations[0],
				Rotation);
			//Rotation.Normalize();
			IsAnimated=true;
		}
	}
	else if(ForceGlobalSequence)
	{		
		if(Rotations.Animations.GetCount())
		{	
			GetInterpolationValue(Time,IsLoop,
				Rotations.InterpolationType,
				0,
				Rotations.Animations[0],
				Rotation);
			//Rotation.Normalize();
			IsAnimated=true;
		}
	}


	if(Scalings.GlobalSequenceID>=0&&Scalings.GlobalSequenceID<(int)GlobalSequences.GetCount())
	{
		if(Scalings.Animations.GetCount())
		{
			GetInterpolationValue(Time,IsLoop,
				Scalings.InterpolationType,
				GlobalSequences[Scalings.GlobalSequenceID],
				Scalings.Animations[0],
				Scaling);
			IsAnimated=true;
		}
	}
	else if(ForceGlobalSequence)
	{
		if(Scalings.Animations.GetCount())
		{
			GetInterpolationValue(Time,IsLoop,
				Scalings.InterpolationType,
				0,
				Scalings.Animations[0],
				Scaling);
			IsAnimated=true;
		}
	}



	if(IsAnimated)
	{
		//TransformMatrix=CD3DMatrix::FromScale(Scaling)*CD3DMatrix::FromRotationQuaternion(Rotation)*CD3DMatrix::FromTranslation(Translation);
		TransformMatrix.SetTransformation(&Scaling,&Rotation,&Translation);
	}

	return IsAnimated;

}

inline bool GetInterpolationTransformWithGlobal(UINT Time,bool IsLoop,UINT AniIndex,UINT AniLength,CD3DMatrix& TransformMatrix,ANIMATION_BLOCK<CD3DVector3>& Translations,ANIMATION_BLOCK<CD3DQuaternion>& Rotations,ANIMATION_BLOCK<CD3DVector3>& Scalings,CEasyArray<UINT>& GlobalSequences)
{

	bool IsAnimated=false;

	CD3DVector3 Translation(0,0,0);
	CD3DQuaternion Rotation;
	CD3DVector3 Scaling(1.0f,1.0f,1.0f);
	Rotation.SetIdentity();


	if(Translations.GlobalSequenceID>=0&&Translations.GlobalSequenceID<(int)GlobalSequences.GetCount())
	{
		if(Translations.Animations.GetCount())
		{
			GetInterpolationValue(Time,true,
				Translations.InterpolationType,
				GlobalSequences[Translations.GlobalSequenceID],
				Translations.Animations[0],
				Translation);
			IsAnimated=true;
		}			
	}
	else if(AniIndex<Translations.Animations.GetCount())
	{
		GetInterpolationValue(Time,IsLoop,
			Translations.InterpolationType,
			AniLength,
			Translations.Animations[AniIndex],
			Translation);
		IsAnimated=true;
	}


	if(Rotations.GlobalSequenceID>=0&&Rotations.GlobalSequenceID<(int)GlobalSequences.GetCount())
	{		
		if(Rotations.Animations.GetCount())
		{	
			GetInterpolationValue(Time,true,
				Rotations.InterpolationType,
				GlobalSequences[Rotations.GlobalSequenceID],
				Rotations.Animations[0],
				Rotation);
			//Rotation.Normalize();
			IsAnimated=true;
		}
	}
	else if(AniIndex<Rotations.Animations.GetCount())
	{	
		GetInterpolationValue(Time,IsLoop,
			Rotations.InterpolationType,
			AniLength,
			Rotations.Animations[AniIndex],
			Rotation);
		//Rotation.Normalize();
		IsAnimated=true;
	}

	if(Scalings.GlobalSequenceID>=0&&Scalings.GlobalSequenceID<(int)GlobalSequences.GetCount())
	{
		if(Scalings.Animations.GetCount())
		{
			GetInterpolationValue(Time,true,
				Scalings.InterpolationType,
				GlobalSequences[Scalings.GlobalSequenceID],
				Scalings.Animations[0],
				Scaling);
			IsAnimated=true;
		}
	}
	else if(AniIndex<Scalings.Animations.GetCount())
	{
		GetInterpolationValue(Time,IsLoop,
			Scalings.InterpolationType,
			AniLength,
			Scalings.Animations[AniIndex],
			Scaling);
		IsAnimated=true;
	}


	if(IsAnimated)
		//TransformMatrix=CD3DMatrix::FromScale(Scaling)*CD3DMatrix::FromRotationQuaternion(Rotation)*CD3DMatrix::FromTranslation(Translation);
		TransformMatrix.SetTransformation(&Scaling,&Rotation,&Translation);


	return IsAnimated;

}

//template<class KEY_TYPE>
//bool PackAnimationBlock(ANIMATION_BLOCK<KEY_TYPE>& AniBlock,CSmartStruct& Packet,WORD MemberID);
//template<class KEY_TYPE>
//bool PackFakeAnimationBlock(FAKE_ANIMATION_FRAME<KEY_TYPE>& AniBlock,CSmartStruct& Packet,WORD MemberID);
//UINT GetPacketAniBlockCount(CSmartStruct& Packet);
//template<class KEY_TYPE>
//bool UnpackAnimationBlock(ANIMATION_BLOCK<KEY_TYPE>& AniBlock,CSmartStruct& Packet,WORD MemberID);
//template<class KEY_TYPE>
//bool UnpackFakeAnimationBlock(FAKE_ANIMATION_FRAME<KEY_TYPE>& AniBlock,CSmartStruct& Packet,WORD MemberID);
//template<class KEY_TYPE>
//UINT GetAnimationBlockSmartStructSize(ANIMATION_BLOCK<KEY_TYPE>& AniBlock);
//template<class KEY_TYPE>
//UINT GetFakeAnimationBlockSmartStructSize(FAKE_ANIMATION_FRAME<KEY_TYPE>& AniBlock);
//template<class KEY_TYPE>
//UINT CaculateAnimationBlockSize(ANIMATION_BLOCK<KEY_TYPE>& AniBlock);

template<class KEY_TYPE>
inline bool PackAnimationBlock(ANIMATION_BLOCK<KEY_TYPE>& AniBlock,CSmartStruct& Packet,WORD MemberID)
{
	UINT BufferSize;
	void * pBuffer=Packet.PrepareMember(BufferSize);
	CSmartStruct AniPacket(pBuffer,BufferSize,true);	

	CHECK_SMART_STRUCT_ADD_AND_RETURN(AniPacket.AddMember(SST_AB_INTERPOLATION_TYPE,AniBlock.InterpolationType));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(AniPacket.AddMember(SST_AB_GLOBALSEQUENCE_ID,AniBlock.GlobalSequenceID));
	for(UINT i=0;i<AniBlock.Animations.GetCount();i++)
	{		
		pBuffer=AniPacket.PrepareMember(BufferSize);
		CSmartStruct SubPacket(pBuffer,BufferSize,true);
		if(AniBlock.Animations[i].TimeStamps.GetCount())
		{
			SubPacket.AddMember(SST_AF_TIMESTAMPS,
				(char *)AniBlock.Animations[i].TimeStamps.GetBuffer(),
				AniBlock.Animations[i].TimeStamps.GetCount()*sizeof(UINT));
		}
		if(AniBlock.Animations[i].Keys.GetCount())
		{
			SubPacket.AddMember(SST_AF_KEYS,
				(char *)AniBlock.Animations[i].Keys.GetBuffer(),
				AniBlock.Animations[i].Keys.GetCount()*sizeof(KEY_TYPE));
		}
		if(!AniPacket.FinishMember(SST_AB_ANIMATION_FRAME,SubPacket.GetDataLen()))
			return false;
	}
	if(!Packet.FinishMember(MemberID,AniPacket.GetDataLen()))
		return false;
	return true;
}

template<class KEY_TYPE>
inline bool PackFakeAnimationBlock(FAKE_ANIMATION_FRAME<KEY_TYPE>& AniBlock,CSmartStruct& Packet,WORD MemberID)
{
	UINT BufferSize;
	void * pBuffer=Packet.PrepareMember(BufferSize);
	CSmartStruct AniPacket(pBuffer,BufferSize,true);	

	if(AniBlock.TimeStamps.GetCount())
	{
		AniPacket.AddMember(SST_AF_TIMESTAMPS,
			(char *)AniBlock.TimeStamps.GetBuffer(),
			AniBlock.TimeStamps.GetCount()*sizeof(FLOAT));
	}
	if(AniBlock.Keys.GetCount())
	{
		AniPacket.AddMember(SST_AF_KEYS,
			(char *)AniBlock.Keys.GetBuffer(),
			AniBlock.Keys.GetCount()*sizeof(KEY_TYPE));
	}

	if(!Packet.FinishMember(MemberID,AniPacket.GetDataLen()))
		return false;
	return true;
}

inline UINT GetPacketAniBlockCount(CSmartStruct& Packet)
{	
	void * Pos=Packet.GetFirstMemberPosition();
	UINT AniCount=0;
	while(Pos)
	{
		WORD MemberID;
		CSmartValue SubValue=Packet.GetNextMember(Pos,MemberID);
		if(MemberID==SST_AB_ANIMATION_FRAME)
			AniCount++;
	}
	return AniCount;
}

template<class KEY_TYPE>
inline bool UnpackAnimationBlock(ANIMATION_BLOCK<KEY_TYPE>& AniBlock,const CSmartStruct& Packet,WORD PackMemberID)
{
	CSmartStruct AniPacket;
	if(PackMemberID)
		AniPacket=Packet.GetMember(PackMemberID);
	else
		AniPacket=Packet;

	void * Pos=AniPacket.GetFirstMemberPosition();
	UINT AniIndex=0;
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=AniPacket.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_AB_ANIMATION_FRAME:			
			AniIndex++;
			break;
		}
	}

	AniBlock.Animations.Resize(AniIndex);

	Pos=AniPacket.GetFirstMemberPosition();
	AniIndex=0;
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=AniPacket.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{
		case SST_AB_INTERPOLATION_TYPE:
			AniBlock.InterpolationType=Value;
			break;
		case SST_AB_GLOBALSEQUENCE_ID:
			AniBlock.GlobalSequenceID=Value;
			break;
		case SST_AB_ANIMATION_FRAME:
			{
				CSmartStruct Frames=Value;
				CSmartValue TimeStamps=Frames.GetMember(SST_AF_TIMESTAMPS);
				CSmartValue Keys=Frames.GetMember(SST_AF_KEYS);
				UINT Count=TimeStamps.GetLength()/sizeof(UINT);
				if(Count)
				{
					AniBlock.Animations[AniIndex].TimeStamps.Resize(Count);
					memcpy(AniBlock.Animations[AniIndex].TimeStamps.GetBuffer(),
						(LPCSTR)TimeStamps,sizeof(UINT)*Count);
				}
				Count=Keys.GetLength()/sizeof(KEY_TYPE);
				if(Count)
				{
					AniBlock.Animations[AniIndex].Keys.Resize(Count);
					memcpy(AniBlock.Animations[AniIndex].Keys.GetBuffer(),
						(LPCSTR)Keys,sizeof(KEY_TYPE)*Count);
				}
			}
			AniIndex++;
			break;
		}
	}
	return true;
}

template<class KEY_TYPE>
inline bool UnpackFakeAnimationBlock(FAKE_ANIMATION_FRAME<KEY_TYPE>& AniBlock,const CSmartStruct& Packet,WORD MemberID)
{
	CSmartStruct AniPacket;
	if(MemberID)
		AniPacket=Packet.GetMember(MemberID);
	else
		AniPacket=Packet;
	CSmartValue TimeStamps=AniPacket.GetMember(SST_AF_TIMESTAMPS);
	CSmartValue Keys=AniPacket.GetMember(SST_AF_KEYS);
	UINT Count=TimeStamps.GetLength()/sizeof(FLOAT);
	if(Count)
	{
		AniBlock.TimeStamps.Resize(Count);
		memcpy(AniBlock.TimeStamps.GetBuffer(),
			(LPCTSTR)TimeStamps,sizeof(FLOAT)*Count);
	}
	Count=Keys.GetLength()/sizeof(KEY_TYPE);
	if(Count)
	{
		AniBlock.Keys.Resize(Count);
		memcpy(AniBlock.Keys.GetBuffer(),
			(LPCTSTR)Keys,sizeof(KEY_TYPE)*Count);
	}
	return true;
}

template<class KEY_TYPE>
inline UINT GetAnimationBlockSmartStructSize(ANIMATION_BLOCK<KEY_TYPE>& AniBlock)
{
	UINT Size=0;

	Size+=SMART_STRUCT_FIX_MEMBER_SIZE(sizeof(AniBlock.InterpolationType));
	Size+=SMART_STRUCT_FIX_MEMBER_SIZE(sizeof(AniBlock.GlobalSequenceID));

	for(UINT i=0;i<AniBlock.Animations.GetCount();i++)
	{
		if(AniBlock.Animations[i].TimeStamps.GetCount())
		{
			Size+=SMART_STRUCT_STRING_MEMBER_SIZE(AniBlock.Animations[i].TimeStamps.GetCount()*sizeof(UINT));
		}
		if(AniBlock.Animations[i].Keys.GetCount())
		{
			Size+=SMART_STRUCT_STRING_MEMBER_SIZE(AniBlock.Animations[i].Keys.GetCount()*sizeof(KEY_TYPE));
		}
		Size+=SMART_STRUCT_STRUCT_MEMBER_SIZE(0);
	}
	Size+=SMART_STRUCT_STRUCT_MEMBER_SIZE(0);
	return Size;
}

template<class KEY_TYPE>
inline UINT GetFakeAnimationBlockSmartStructSize(FAKE_ANIMATION_FRAME<KEY_TYPE>& AniBlock)
{
	UINT Size=0;
	if(AniBlock.TimeStamps.GetCount())
	{
		Size+=SMART_STRUCT_STRING_MEMBER_SIZE(AniBlock.TimeStamps.GetCount()*sizeof(FLOAT));
	}
	if(AniBlock.Keys.GetCount())
	{
		Size+=SMART_STRUCT_STRING_MEMBER_SIZE(AniBlock.Keys.GetCount()*sizeof(KEY_TYPE));
	}
	Size+=SMART_STRUCT_STRUCT_MEMBER_SIZE(0);

	return Size;
}

template<class KEY_TYPE>
inline UINT CaculateAnimationBlockSize(ANIMATION_BLOCK<KEY_TYPE>& AniBlock)
{
	UINT DataSize=sizeof(ANIMATION_BLOCK<KEY_TYPE>);
	for(UINT i=0;i<AniBlock.Animations.GetCount();i++)
	{
		DataSize+=sizeof(KEY_TYPE)*AniBlock.Animations[i].Keys.GetCount();
		DataSize+=sizeof(UINT)*AniBlock.Animations[i].TimeStamps.GetCount();
	}
	return DataSize;
}

}