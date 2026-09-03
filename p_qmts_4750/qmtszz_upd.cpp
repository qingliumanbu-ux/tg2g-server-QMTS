/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-28
Description: 修改组合元素公式代码
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 修改组合元素公式代码
/// <para>
/// 1.修改组合元素公式代码
/// </para>
/// <para>数据库表：tep0002(代码值集信息表)                </para>
/// <para>主调用函数：前台QMTSZZ画面的F4(修改)调用。       </para>
/// </summary>
/// <param name="CODE_CLASS"> 代码编号                     </param>
/// <param name="CODE"> 代码                               </param>
/// <param name="CODE_DESC_1_CONTENT"> 代码描述一内容      </param>
===========================================================</remark>*/

/* ***** 外部函数申明 ***** */

// service入口
BM2F_ENTERACE(qmtszz_upd)

int f_qmtszz_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString formula_value = "";

	/* 实体类定义 */
	CModel tep0002("TEP0002");

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_upd(conn);

	try
	{
		/*获得传入参数*/
		tep0002["CODE_CLASS"] = bcls_rec->Tables[0].Rows[0]["CODE_CLASS"];
		tep0002["CODE"] = bcls_rec->Tables[0].Rows[0]["CODE"];
		formula_value = bcls_rec->Tables[0].Rows[0]["CODE_DESC_1_CONTENT"];

		Log::Trace("", "","qmtszz_upd IN:---tep0002.CODE_CLASS = [{0}]",(const char*)tep0002["CODE_CLASS"].ToString());
		Log::Trace("", "","qmtszz_upd IN:---tep0002.CODE = [{0}]",(const char*)tep0002["CODE"].ToString());
		Log::Trace("", "","qmtszz_upd IN:---formula_value = [{0}]",(const char*)formula_value);

		//查询工艺卡表
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT REC_CREATOR,REC_CREATE_TIME "
				"   FROM TEP0002 "
				"  WHERE CODE_CLASS = @tep0002.CODE_CLASS "
				"    AND CODE = @tep0002.CODE ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tep0002.CODE_CLASS", tep0002["CODE_CLASS"].ToString());
		cmd_inq.Parameters.Set("tep0002.CODE", tep0002["CODE"].ToString());
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			tep0002["REC_CREATOR"] = cmd_inq.GetString(1);
			tep0002["REC_CREATE_TIME"] = cmd_inq.GetString(2);
		}
		cmd_inq.Close();

		tep0002["REC_REVISOR"] = s.userid;
		tep0002["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

		if(formula_value.Find("=", 0) > 0)
		{
			tep0002["CODE_DESC_1_CONTENT"] = formula_value.Substring(formula_value.Find("=", 0)+1);
			tep0002["CODE_DESC_3_CONTENT"] = " ";
			tep0002["CODE_DESC_4_CONTENT"] = " ";
		}
		else if(formula_value.Find(">", 0) > 0)
		{
			tep0002["CODE_DESC_1_CONTENT"] = formula_value.Substring(0,formula_value.Find(">", 0));
			tep0002["CODE_DESC_3_CONTENT"] = ">";
			tep0002["CODE_DESC_4_CONTENT"] = formula_value.Substring(formula_value.Find(">", 0)+1);
		}
		else if(formula_value.Find("<", 0) > 0)
		{
			tep0002["CODE_DESC_1_CONTENT"] = formula_value.Substring(0,formula_value.Find("<", 0));
			tep0002["CODE_DESC_3_CONTENT"] = "<";
			tep0002["CODE_DESC_4_CONTENT"] = formula_value.Substring(formula_value.Find("<", 0)+1);
		}
		else
		{
			tep0002["CODE_DESC_1_CONTENT"] = formula_value;
			tep0002["CODE_DESC_3_CONTENT"] = " ";
			tep0002["CODE_DESC_4_CONTENT"] = " ";
		}

		//删除工艺卡表
		tep0002.Delete("CODE_CLASS,CODE");
		//新增工艺卡表
		tep0002.Insert();
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
