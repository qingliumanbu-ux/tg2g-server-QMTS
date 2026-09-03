/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      guxia
Version:     1.0
Date:        2015-07-03
Description: 炉次质量信息新增，供炼钢计划在转炉产出时调用
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件，请包含在""中
BM2_FUNCTION_EXPORT
 int f_qmts23_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts23("TQMTS23");
	CModel tqmts29("TQMTS29");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//************************************************* 获取传入参数  *************************************************//
		tqmts23.MergeFrom(bcls_rec->Tables[0].Rows[i]);

		Log::Trace("", __FUNCTION__, "f_qmts23_ins IN:---tqmts23.HEAT_NO = [{0}]", tqmts23["HEAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "f_qmts23_ins IN:---tqmts23.PONO = [{0}]", tqmts23["PONO"].ToString());

		//校核
		if (tqmts23["HEAT_NO"].ToString().TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0004334")/*熔炼号不允许为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tqmts23["PONO"].ToString().TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("MM00S0000072")/*制造命令号不能为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		fetchRowCount = 0;
		tqmts29["HEAT_NO"] = tqmts23["HEAT_NO"];
		tqmts29["PONO"] = tqmts23["PONO"];
		fetchRowCount = tqmts29.QueryCount("HEAT_NO,PONO");
		Log::Trace("", __FUNCTION__, "fetchRowCount= [{0}]", fetchRowCount);
		if (fetchRowCount <= 0)//有代表成分以后，不再更新tqmts23
		{

			tqmts23["REC_CREATOR"] = s.userid;
			tqmts23["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts23["REC_REVISOR"] = " ";
			tqmts23["REC_REVISE_TIME"] = " ";
			tqmts23["ARCHIVE_FLAG"] = " ";

			//写入数据
			tqmts23.Delete("HEAT_NO");
			tqmts23.Insert();
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}

