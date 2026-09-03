/*<remark>=========================================================
/// <summary>
/// 画面检核登记表-新增
/// <para>新增画面检核登记表</para>
/// </summary>
/// <param name="输入">TQMTMA8表结构</param>
/// <returns>成败标记</returns>
===========================================================</remark>*/
/// <summary>
/// <version>1.0</version>
/// <history>2011-11-14 文件创建</history>
/// </summary>

#include "stdafx.h"

// Service 入口
BM2F_ENTERACE(qmtsa8_ins)

///////////////////////////////////////////////////
//执行新增
///////////////////////////////////////////////////
int f_qmtsa8_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序用变量
	int doFlag = 0;
	int i = 0;
	/* 实体类定义 */
	CModel tqmtma8("TQMTMA8");
	CString sqlstr = "";

	try
	{
		//初始化
		s.flag = 0;
		s.sqlcode = 0;
		strcpy(s.msg, " ");

		//对输入信息循环处理
		for (i = 1; i <= bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tqmtma8.Reset();
			//取得单行传入信息
			tqmtma8.MergeFrom(bcls_rec->Tables[0].Rows[i - 1]);

			//赋初值			
			tqmtma8["VERSION"] = 1;
			tqmtma8["REC_CREATOR"] = s.userid;
			CDateTime timeNow = CDateTime::Now();
			tqmtma8["REC_REVISOR"] = " ";
			tqmtma8["REC_REVISE_TIME"] = " ";
			tqmtma8["ARCHIVE_FLAG"] = " ";
			tqmtma8["DU_FLAG"] = " ";
			tqmtma8["DU_MAKER"] = " ";
			tqmtma8["DU_TIME"] = " ";
			
			Log::Trace("", "", "ST_NO[{0}]", (const char*)tqmtma8["ST_NO"].ToString());
			Log::Trace("", "", "SG_SIGN1[{0}]", (const char*)tqmtma8["SG_SIGN1"].ToString());
			Log::Trace("", "", "REMARK[{0}]", (const char*)tqmtma8["REMARK"].ToString());
		/*	Log::Trace("", "", "SG_SIGN2[{0}]", (const char*)tqmtma8.SG_SIGN2);
			Log::Trace("", "", "SG_SIGN3[{0}]", (const char*)tqmtma8.SG_SIGN3);
			Log::Trace("", "", "SG_SIGN4[{0}]", (const char*)tqmtma8.SG_SIGN4);
			Log::Trace("", "", "SG_SIGN5[{0}]", (const char*)tqmtma8.SG_SIGN5);
			Log::Trace("", "", "SG_SIGN6[{0}]", (const char*)tqmtma8.SG_SIGN6);
			Log::Trace("", "", "SG_SIGN7[{0}]", (const char*)tqmtma8.SG_SIGN7);
			Log::Trace("", "", "SG_SIGN8[{0}]", (const char*)tqmtma8.SG_SIGN8);
			Log::Trace("", "", "SG_SIGN9[{0}]", (const char*)tqmtma8.SG_SIGN9);*/
			//执行新增
			tqmtma8.Insert();

		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}