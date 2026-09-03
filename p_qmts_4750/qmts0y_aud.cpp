/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-11
Description: 审核工艺卡
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中
#include "tqmts0y.h"

/*<remark>=========================================================
/// <summary>
/// 审核工艺卡
/// <para>
/// 1.审核工艺卡和对应的成分标准信息
/// </para>
/// <para>数据库表：TQMTS0Y(工艺卡表);
//                  TQMTS02(工序成分标准表)                </para>
/// <para>主调用函数：前台QMTS0Y画面的F8(审核)调用。       </para>
/// </summary>
/// <param name="SG_CODE"> 出钢记号                          </param>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts0y_aud)

int f_qmts0y_aud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CDecimal i_count = 0;

	/* 实体类定义 */
	CTQMTS0Y tqmts0y(conn);

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		tqmts0y.SG_CODE = bcls_rec->Tables[0].Rows[0]["SG_CODE"].ToString().Trim();
		tqmts0y.HEAT_ELM_IDX = bcls_rec->Tables[0].Rows[0]["HEAT_ELM_IDX"].ToString().Trim();

		Log::Trace("", "","qmts0y_aud IN:---SG_CODE = [{0}]", (const char*)tqmts0y.SG_CODE);	
		Log::Trace("", "","qmts0y_del IN:---HEAT_ELM_IDX = [{0}]",(const char*)tqmts0y.HEAT_ELM_IDX);

		//校验出钢记号是否已下发——add by 冯晓轶 2012-04-26
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT VALID_FLAG "
				"   FROM TQMTS0Y "						   
				"  WHERE SG_CODE = @tqmts0y.SG_CODE ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts0y.SG_CODE", tqmts0y.SG_CODE);
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			tqmts0y.VALID_FLAG = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		if(tqmts0y.VALID_FLAG == "2")
		{
			strcpy(s.msg,_RES("QM00S0005920")/*该出钢记号已下发，不可做审核。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TQMTS0Y "
				"  WHERE SG_CODE = @tqmts0y.SG_CODE "
				"  AND HEAT_ELM_IDX = @tqmts0y.HEAT_ELM_IDX";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts0y.SG_CODE", tqmts0y.SG_CODE);
		cmd_inq.Parameters.Set("tqmts0y.HEAT_ELM_IDX", tqmts0y.HEAT_ELM_IDX);
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts0y);
		}
		cmd_inq.Close();
		//if(tqmts0y.SMELT_DIV.TrimOrBlank() == " ")
		//{
		//	strcpy(s.msg,_RES("QM00S0005922")/*审核失败：该出钢记号工艺卡缺少冶炼区分。*/);
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//if(tqmts0y.REFINE_ROUTE_CODE.TrimOrBlank() == " ")
		//{
		//	strcpy(s.msg,_RES("QM00S0005923")/*审核失败：该出钢记号工艺卡缺少精炼路径。*/);
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//if(tqmts0y.LABEL1.TrimOrBlank() == " "
		//	&& tqmts0y.LABEL2.TrimOrBlank() == " "
		//	&& tqmts0y.LABEL3.TrimOrBlank() == " "
		//	&& tqmts0y.LABEL4.TrimOrBlank() == " "
		//	&& tqmts0y.LABEL5.TrimOrBlank() == " ")
		//{
		//	strcpy(s.msg,_RES("QM00S0005924")/*审核失败：该出钢记号工艺卡缺少适用牌号。*/);
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		tqmts0y.VALID_FLAG = "1";
		tqmts0y.CHECK_MAKER = s.userid;
		tqmts0y.CHECK_TIME = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tqmts0y.Update("VALID_FLAG,CHECK_TIME,CHECK_MAKER",
			"SG_CODE, HEAT_ELM_IDX");
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
