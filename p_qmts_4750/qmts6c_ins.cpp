/*<remark>=========================================================
/// <summary>
/// 画面降级改判作业表-新增
/// <para>降级改判作业表</para>
/// </summary>
/// <param name="输入">TQMTS6C表结构</param>
/// <returns>成败标记</returns>
===========================================================</remark>*/
/// <summary>
/// <version>1.0</version>
/// <history>2011-11-14 文件创建</history>
/// </summary>

#include "stdafx.h"

// Service 入口
BM2F_ENTERACE(qmts6c_ins)

///////////////////////////////////////////////////
//执行新增
///////////////////////////////////////////////////
int f_qmts6c_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序用变量
	int doFlag = 0;
	int i = 0;
	/* 实体类定义 */
	CModel tqmts6c("TQMTS6C");
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
			tqmts6c.Reset();
			//取得单行传入信息
			tqmts6c.MergeFrom(bcls_rec->Tables[0].Rows[i - 1]);

			//赋初值			
			tqmts6c["VERSION"] = 1;
			tqmts6c["REC_CREATOR"] = s.userid;
			CDateTime timeNow = CDateTime::Now();
			tqmts6c["REC_REVISOR"] = " ";
			tqmts6c["REC_REVISE_TIME"] = " ";
			tqmts6c["ARCHIVE_FLAG"] = " ";
			tqmts6c["DU_FLAG"] = " ";
			tqmts6c["DU_MAKER"] = " ";
			tqmts6c["DU_TIME"] = " ";
			
		/*	Log::Trace("", "", "ST_NO[{0}]", (const char*)tqmtma8["ST_NO"].ToString());
			Log::Trace("", "", "SG_SIGN1[{0}]", (const char*)tqmtma8["SG_SIGN1"].ToString());
			Log::Trace("", "", "REMARK[{0}]", (const char*)tqmtma8["REMARK"].ToString());*/

			//执行新增
			tqmts6c.Insert();

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