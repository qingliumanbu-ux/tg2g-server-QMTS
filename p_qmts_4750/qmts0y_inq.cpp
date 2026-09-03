/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-11
Description: 查询工艺卡
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中
#include "tqmts0y.h"
#include "tqmts02.h"
#include "tep0002.h"

/*<remark>=========================================================
/// <summary>
/// 查询工艺卡
/// <para>
/// 1.查询工艺卡和对应的成分标准信息
/// </para>
/// <para>数据库表：TQMTS0Y(工艺卡表);
//                  TQMTS02(工序成分标准表)                </para>
/// <para>主调用函数：前台QMTS0Y画面的F2(查询)调用。       </para>
/// </summary>
/// <param name="ST_NO"> 出钢记号                          </param>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts0y_inq)

int f_qmts0y_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CDecimal q_flag = 0;
	CString q_heat_elm_idx("");
	CString q_sg_code("");
	CString q_sg_sign("");


	/* 实体类定义 */
	CTQMTS0Y tqmts0y(conn);
	CTQMTS02 tqmts02(conn);
	CTEP0002 tep0002(conn);

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_tep0002(conn);

	try
	{
		/*获得传入参数*/
		q_flag = bcls_rec->Tables[0].Rows[0]["FLAG"].ToDecimal();
		q_sg_code = bcls_rec->Tables[0].Rows[0]["SG_CODE"].ToString().Trim();
		q_heat_elm_idx = bcls_rec->Tables[0].Rows[0]["HEAT_ELM_IDX"].ToString().Trim();

		Log::Trace("", "","qmts0y_inq IN:---FLAG = [{0}]",q_flag.ToInt32());
		Log::Trace("", "","qmts0y_inq IN:---SG_CODE = [{0}]",(const char*)q_sg_code);

		//q_heat_elm_idx = "A";

		if(q_flag == 0)
		{
			q_sg_sign  = bcls_rec->Tables[0].Rows[0]["sg_sign"].ToString().Trim();

			Log::Trace("", "","qmts0y_inq IN:---SG_SIGN = [{0}]",(const char*)q_sg_sign);

			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT DISTINCT SG_CODE, SG_SIGN, VALID_FLAG, HEAT_ELM_IDX "
					"   FROM TQMTS0Y "						   
					"  WHERE SG_CODE LIKE @q_sg_code||'%' "
					"    AND HEAT_ELM_IDX LIKE '%' || @q_heat_elm_idx || '%' "
					"    AND SG_SIGN LIKE '%' || @q_sg_sign || '%' "
					" ORDER BY SG_CODE ASC ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("q_heat_elm_idx", q_heat_elm_idx);
			cmd_inq.Parameters.Set("q_sg_code", q_sg_code);
			cmd_inq.Parameters.Set("q_sg_sign", q_sg_sign);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		else
		{
			if(q_sg_code.Trim()=="")
			{
				strcpy(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//获取工艺卡标准
			//增加块名QMTSBLK01
			bcls_ret->Tables.Add("QMTSBLK01");
			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
					"   FROM TQMTS0Y "
					"  WHERE HEAT_ELM_IDX = @q_heat_elm_idx "
					"    AND SG_CODE = @q_sg_code ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("q_heat_elm_idx", q_heat_elm_idx);
			cmd_inq.Parameters.Set("q_sg_code", q_sg_code);
			cmd_inq.ExecuteQuery(bcls_ret->Tables["QMTSBLK01"]);
			cmd_inq.Close();

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
