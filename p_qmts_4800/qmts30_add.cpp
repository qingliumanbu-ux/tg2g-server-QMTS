/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-01-15
Description: 滞溜坯处置导入
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(qmts30_add)

int f_qmts30_add(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");



	/* 业务变量 */
	CModel tqmts30("TQMTS30");
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_inq(conn);
	CDecimal cd_count = 0;
	/* 数据库操作类定义 */

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tqmts30.Reset();
			tqmts30.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			CString mat_no = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "mat_no	= [{0}]", (const char*)mat_no);

			//查询是否有重复数据
			CString	sqlzr = "SELECT COUNT(*) FROM TQMTS30 WHERE MAT_NO ='" + mat_no + "'";
			Log::Trace("", __FUNCTION__, "sqlzr	= [{0}]", (const char*)sqlzr);
			cmd_inq.Parameters.Clear();
			cmd_inq.SetCommandText(sqlzr);
			cd_count = cmd_inq.ExecuteScalar();
			Log::Trace("", __FUNCTION__, "cd_count	= [{0}]", cd_count);
			if (cd_count > 0)
			{
				Log::Trace("", __FUNCTION__, "巴拉巴拉1");
				sprintf(s.msg, mat_no + "改材料号已存在，不可导入！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq.Close();

			Log::Trace("", "", "111");

			if (bcls_rec->Tables[0].Rows[i]["AREA"].ToString().Trim() == "南区" && mat_no.Substring(0, 2) == "B3")
			{
				tqmts30["C_DIV"] = "1";
			}
			if (bcls_rec->Tables[0].Rows[i]["AREA"].ToString().Trim() == "南区" && mat_no.Substring(0, 2) != "B3")
			{
				tqmts30["C_DIV"] = "2";
			}

			tqmts30["REC_CREATOR"] = s.userid;
			tqmts30["REC_CREATE_TIME"] = datetime;
			tqmts30["REPORT_TIME"] = datetime;
			tqmts30["REC_REVISOR"] = " ";
			tqmts30["REC_REVISE_TIME"] = " ";
			tqmts30["STATUS_FLAG"] = "0";//判定状态 未判
			tqmts30["FULL_FURNACE"] = "否";
			tqmts30["DECIDER"] = s.username;
			tqmts30.TrimOrBlank();
			tqmts30.Insert();

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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


