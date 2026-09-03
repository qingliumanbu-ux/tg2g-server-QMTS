/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      顾霞
Version:     1.0
Date:        2014-10-30
Description: 查询内部钢种
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 查询各个制造标准
/// <para>
/// 获取输入参数：st_no(出钢记号)/whole_backlog_code(炼钢工序)
/// </para>
/// <para>数据库表：
/// 				TQMTS02(工序成分标准表)
/// 前台各个QMTS0x画面的F2(查询)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各制造标准表    </param>
===========================================================</remark>*/

int f_qmtsp_00(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//校验工艺卡是否已更新

// service入口
BM2F_ENTERACE(qmts_inqchemi)


int f_qmts_inqchemi(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;
	int fetchRowCount;
	CDecimal v_count = 0;

	CString s_st_no = " ";
	CString s_whole_backlog_code = " ";
	CString factory_div = "";
	CString base_code = "";

	/* 实体类定义 */
	CModel tep0002("TEP0002");
	CModel tqmts02("TQMTS02");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_02(conn);

	bcls_ret->Tables[0].Columns.Add(tqmts02);

	try
	{
		/*获得传入参数*/
		s_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		s_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["whole_backlog_code"].ToString().Trim();
		factory_div= bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().Trim();
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();

		Log::Trace("", "", "qmts_inqstno IN: st_no[{0}]whole_backlog_code[{1}]base_code[{2}]", (const char*)s_st_no, (const char*)s_whole_backlog_code, (const char*)factory_div,(const char*)base_code);

		if (s_st_no.TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0006260")/*内部钢种不能为空。*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//获取成分标准
		fetchRowCount = 0;
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * "
				"  FROM TEP0002 "
				" WHERE CODE_CLASS = 'QMYS' "
				"   AND TRIM(CODE_DESC_2_CONTENT) IS NOT null "
				" ORDER BY CODE_DESC_2_CONTENT ASC ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tep0002);
			fetchRowCount++;

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
					"  FROM TQMTS02 "
					" WHERE ST_NO = @st_no"
					"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code"
					"   AND FACTORY_DIV = @factory_div"
					"   AND BASE_CODE = @base_code "
					"   AND ELM_CODE = @elm_code";
				break;
			}
			cmd_inq_02.SetCommandText(sqlstr);
			cmd_inq_02.Parameters.Set("st_no", s_st_no);
			cmd_inq_02.Parameters.Set("whole_backlog_code", s_whole_backlog_code);
			cmd_inq_02.Parameters.Set("factory_div", factory_div);
			cmd_inq_02.Parameters.Set("base_code", base_code);
			cmd_inq_02.Parameters.Set("elm_code", tep0002["CODE"].ToString());
			cmd_inq_02.ExecuteReader();
			if (cmd_inq_02.Read())
			{
				cmd_inq_02.Fetch(tqmts02);
			}
			else
			{
				tqmts02["ELM_CODE"] = tep0002["CODE"];
				tqmts02["ELM_NAME"] = tep0002["CODE_DESC_1_CONTENT"];
				tqmts02["MAIN_MIN"] = 0;
				tqmts02["MAIN_MAX"] = 99.999;
				tqmts02["MAIN_AIM"] = 0;
				tqmts02["SPE_MIN"] = 0;
				tqmts02["SPE_MAX"] = 99.999;
				tqmts02["SMELT_CHEMI_FLAG"] = "";
				tqmts02["ELM_ACCU"] = 0;
			}
			cmd_inq_02.Close();
			if (tqmts02["ELM_ACCU"].ToDecimal() != 0)
			{
				Log::Trace("", "", "tqmts02.ELM_ACCU[{0}]", tqmts02["ELM_ACCU"].ToDecimal());
				tqmts02["MAIN_MIN"] = tqmts02["MAIN_MIN"].ToDecimal().Round(tqmts02["ELM_ACCU"].ToDecimal().ToInt32());
				tqmts02["MAIN_MAX"] = tqmts02["MAIN_MAX"].ToDecimal().Round(tqmts02["ELM_ACCU"].ToDecimal().ToInt32());
				tqmts02["MAIN_AIM"] = tqmts02["MAIN_AIM"].ToDecimal().Round(tqmts02["ELM_ACCU"].ToDecimal().ToInt32());
				tqmts02["SPE_MIN"] = tqmts02["SPE_MIN"].ToDecimal().Round(tqmts02["ELM_ACCU"].ToDecimal().ToInt32());
				tqmts02["SPE_MAX"] = tqmts02["SPE_MAX"].ToDecimal().Round(tqmts02["ELM_ACCU"].ToDecimal().ToInt32());
			}
			CDataRow& row = bcls_ret->Tables[0].Rows.Add();   //新增空行
			row.Merge(tqmts02);   //将实体类的值写入新增行中
		}
		cmd_inq.Close();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", "", "error=[{0}]", (const char*)str);

		strncpy(s.sysmsg, (const char*)str, 399);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
