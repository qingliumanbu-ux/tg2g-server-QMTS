/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   bhy
Version:    1.0
Date:     2024-08-26 9:13:56
Description: 核电熔炼报告手动增加成分
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(qmts0r05_add)

int f_qmts0r05_add(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CModel tqmts0r05("TQMTS0R05");
	CString v_operate = "";
	CString sqlstr = "";
	CString v_heat_no = "";
	CString v_order_no = "";
	CString v_now_row = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	CDbCommand cmd_inq(conn);

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tqmts0r05.Reset();
			tqmts0r05.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (v_operate == "UPD")
			{
				tqmts0r05["REC_REVISOR"] = s.userid;
				tqmts0r05["REC_REVISE_TIME"] = datetime;
				tqmts0r05["DECIDE_MAKER"] = s.username;
				tqmts0r05["JUDGE_TIME"] = datetime;

				//tqmts0r05.TrimOrBlank();
				//tqmts0r05.Update("REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME,RES_PROCESS,DECIDE_MAKER,CK_REMARK,JUDGE_TIME,DECIDER,STATUS_FLAG,FULL_FURNACE", "HEAT_NO,ST_NO,MAT_NO");
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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
