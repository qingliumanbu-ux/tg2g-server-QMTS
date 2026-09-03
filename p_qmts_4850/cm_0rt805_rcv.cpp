/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      质保书修约信息表
Version:     1.0
Date:        2024-10-25
Description: 0RT805
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 质保书修约信息表
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/
 


//外部函数声明
int f_tableObjectCheck9999(ITableObject2& obj);	//字段超长检测

BM2F_ENTERACE_TELE(cm_0rt805_rcv)

int f_cm_0rt805_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	/* 业务变量 */

	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_test(conn);
	CModel tqmts0r05("TQMTS0R05");

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		tqmts0r05.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		f_tableObjectCheck9999(tqmts0r05);
		tqmts0r05["AYL_IMAGE"] = "http://10.162.72.16:10004/DiBei/MR.png";
		tqmts0r05["DECIDE_IMAGE"] = "http://10.162.72.16:10004/DiBei/MR.png";
		tqmts0r05["CHECK_IMAGE"] = "http://10.162.72.16:10004/DiBei/MR.png";
		tqmts0r05["MANUAL_FLAG01"] = "0";
		tqmts0r05["MANUAL_FLAG02"] = "0";
		tqmts0r05["MANUAL_FLAG03"] = "0";
		tqmts0r05["MANUAL_FLAG04"] = "0";
		tqmts0r05["MANUAL_FLAG05"] = "0";
		tqmts0r05["MANUAL_FLAG06"] = "0";
		tqmts0r05["MANUAL_FLAG07"] = "0";
		tqmts0r05["MANUAL_FLAG08"] = "0";
		tqmts0r05["MANUAL_FLAG09"] = "0";
		tqmts0r05["MANUAL_FLAG10"] = "0";
		tqmts0r05["MANUAL_FLAG11"] = "0";
		tqmts0r05["MANUAL_FLAG12"] = "0";
		tqmts0r05["MANUAL_FLAG13"] = "0";
		tqmts0r05["MANUAL_FLAG14"] = "0";
		tqmts0r05["MANUAL_FLAG15"] = "0";
		tqmts0r05["MANUAL_FLAG16"] = "0";
		tqmts0r05["MANUAL_FLAG17"] = "0";
		tqmts0r05["MANUAL_FLAG18"] = "0";
		tqmts0r05["MANUAL_FLAG19"] = "0";
		tqmts0r05["MANUAL_FLAG20"] = "0";
		tqmts0r05["MANUAL_FLAG21"] = "0";
		tqmts0r05["MANUAL_FLAG22"] = "0";
		tqmts0r05["MANUAL_FLAG23"] = "0";
		tqmts0r05["MANUAL_FLAG24"] = "0";
		tqmts0r05["MANUAL_FLAG25"] = "0";
		tqmts0r05["MANUAL_FLAG26"] = "0";
		tqmts0r05["MANUAL_FLAG27"] = "0";
		tqmts0r05["MANUAL_FLAG28"] = "0";
		tqmts0r05["MANUAL_FLAG29"] = "0";
		tqmts0r05["MANUAL_FLAG30"] = "0";

		//if (tqmts0r05.QueryCount("HEAT_NO,ORDER_NO") > 0){
			//tqmts0r05.Delete("HEAT_NO,ORDER_NO");
			tqmts0r05["REC_CREATOR"] = s.userid;
			tqmts0r05["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

			CString v_heat_no = tqmts0r05["HEAT_NO"].ToString();
			CString v_order_no = tqmts0r05["ORDER_NO"].ToString();
			sqlstr = "SELECT DECODE(max(now_row),null,0,max(now_row)) FROM TQMTS0R05 where heat_no='" + v_heat_no + "'and order_no='" + v_order_no + "'";
			cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.ExecuteNonQuery();
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts0r05["NOW_ROW"] = cmd_inq.GetDecimal(1) + 1;
				Log::Trace("", __FUNCTION__, "now_row =[{0}]", tqmts0r05["NOW_ROW"].ToString());

			}

			tqmts0r05.TrimOrBlank();
			tqmts0r05.Insert();
		//}
		//else{
		//	tqmts0r05.Insert();
		//}
		//sprintf(s.msg, "%d条记录新增成功！请重新查询！");
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


