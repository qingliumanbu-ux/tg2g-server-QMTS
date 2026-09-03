/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-28
Description: 新增组合元素公式代码
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 新增组合元素公式代码
/// <para>
/// 1.新增组合元素公式代码
/// </para>
/// <para>数据库表：tep0002(代码值集信息表)                </para>
/// <para>主调用函数：前台QMTSZZ画面的F3(新增)调用。       </para>
/// </summary>
/// <param name="CODE_CLASS"> 代码编号                     </param>
/// <param name="CODE_DESC_1_CONTENT"> 代码描述一内容      </param>
===========================================================</remark>*/

/* ***** 外部函数申明 ***** */

// service入口
BM2F_ENTERACE(qmtszz_ins)

int f_qmtszz_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CDecimal i_count = 0;
	CDecimal code = 0;
	CString formula_value = "";

	/* 实体类定义 */
	CModel tep0002("TEP0002");

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_ins(conn);

	try
	{
		/*获得传入参数*/
		tep0002["CODE_CLASS"] = bcls_rec->Tables[0].Rows[0]["CODE_CLASS"];
		formula_value = bcls_rec->Tables[0].Rows[0]["CODE_DESC_1_CONTENT"];

		Log::Trace("", "","qmtszz_ins IN:---tep0002.CODE_CLASS = [{0}]",(const char*)tep0002["CODE_CLASS"].ToString());
		Log::Trace("", "","qmtszz_ins IN:---formula_value = [{0}]",(const char*)formula_value);

		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT COUNT(1) "
				"   FROM TEP0002_RES "						   
				"  WHERE CODE_CLASS = @tep0002.CODE_CLASS "
				"    AND CODE_DESC_1_CONTENT = @formula_value";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tep0002.CODE_CLASS", tep0002["CODE_CLASS"].ToString());
		cmd_inq.Parameters.Set("formula_value", formula_value);
		i_count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if(i_count > 0)
		{
			CFormattable arguments[] = {(const char*)formula_value};
			CMessageFormat::Format(s.msg,_RES("QM00S0005929")/*公式[{0}]已存在，不能再新增。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT MAX(CODE) "
				"   FROM TEP0002 "						   
				"  WHERE CODE_CLASS = @tep0002.CODE_CLASS ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tep0002.CODE_CLASS", tep0002["CODE_CLASS"].ToString());
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			code = cmd_inq.GetDecimal(1)+1;
		}
		else
		{
			code = 1;
		}
		if (code.ToString().GetLength() == 1)
		{
			tep0002["CODE"] = "0" + code.ToString().Trim();
		}
		else
		{
			tep0002["CODE"] = code.ToString().Trim();
		}
		Log::Trace("", "","tep0002.CODE  = [{0}]",(const char*)tep0002["CODE"].ToString());
		cmd_inq.Close();

		tep0002["REC_CREATOR"] = s.userid;
		tep0002["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

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

		tep0002.Insert();

		//switch(conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:				// MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default:						// 所有数据库适用，通用SQL语句
		//	sqlstr = " INSERT INTO TEP0002_RES "
		//			"        (CULTURE,CODE_CLASS,CODE,CODE_DESC_1_CONTENT,CODE_DESC_2_CONTENT,CODE_DESC_3_CONTENT,CODE_DESC_4_CONTENT,CODE_DESC_5_CONTENT) "
		//			"  VALUES('en',@tep0002["CODE_CLASS"].ToString(),@tep0002["CODE"].ToString(),@formula_value,'~ ','~ ','~ ','~ ' ) ";
		//	break;
		//}
		//cmd_ins.SetCommandText(sqlstr);
		//cmd_ins.Parameters.Set("tep0002.CODE_CLASS", tep0002["CODE_CLASS"].ToString());
		//cmd_ins.Parameters.Set("tep0002.CODE", tep0002["CODE"].ToString());
		//cmd_ins.Parameters.Set("formula_value", formula_value);
		//cmd_ins.ExecuteScalar( );
		//cmd_ins.Close();

		//tep0002_res.CODE_CLASS = tep0002["CODE_CLASS"];
		//tep0002_res.CODE = tep0002["CODE"];
		//tep0002_res.CODE_DESC_1_CONTENT = formula_value;
		//tep0002_res.CULTURE = "en";
		//tep0002_res.Insert();
		//tep0002_res.CULTURE = "zh_Hans";
		//tep0002_res.Insert();
		//tep0002_res.CULTURE = "zh_Hant";
		//tep0002_res.Insert();
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
