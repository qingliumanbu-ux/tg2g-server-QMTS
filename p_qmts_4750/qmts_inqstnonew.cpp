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
/// 获取输入参数：table_name(制造标准表名)
/// </para>
/// <para>数据库表：TQMTS01(工序制造标准表)
///                 TQMTS0X(工艺卡)
/// 前台各个QMTS0x画面的F2(查询)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各制造标准表    </param>
===========================================================</remark>*/ 

int f_qmtsp_00(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//校验工艺卡是否已更新

// service入口
BM2F_ENTERACE(qmts_inqstnonew)


int f_qmts_inqstnonew(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;
	int fetchRowCount;
	CDecimal v_count = 0;

	CString table_name = " ";
	CString s_whole_backlog_code = " ";
	CString base_code = "";

	/* 实体类定义 */
	CModel tqmts01("TQMTS01");
	CModel tqmts0x("TQMTS0X");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);

	//查询出钢记号
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "STD_FLAG");      //检查标准是否存在
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");      //update by yiling 20170224  加厂别
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "BASE_CODE");
//	bcls_ret->Tables[0].Columns.Add(DT_STRING,"st_no");
//	bcls_ret->Tables[0].Columns.Add(DT_STRING,"std_flag");      //检查标准是否存在
//	bcls_ret->Tables[0].Columns.Add(DT_STRING, "factory_div");      //update by yiling 20170224  加厂别

	try
	{
		/*获得传入参数*/
		table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().Trim();
		s_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["whole_backlog_code"].ToString().Trim();
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();

		Log::Trace("", "", "qmts_inqstno IN: table_name[{0}]whole_backlog_code[{1}]base_code[{2}]", (const char*)table_name, (const char*)s_whole_backlog_code,(const char*)base_code);

		//从工艺卡表(TQMTS0X)中查询新出钢记号
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT DISTINCT ST_NO,FACTORY_DIV,BASE_CODE "
				" FROM TQMTS0X "
				" WHERE (ST_NO,FACTORY_DIV,BASE_CODE) NOT IN (SELECT DISTINCT ST_NO,FACTORY_DIV,BASE_CODE   FROM " + table_name + ") "
				"     AND VALID_FLAG = '1' "
				;

			if (table_name == "TQMTS08")
			{
				sqlstr = sqlstr + " AND IC_CC_FLAG = 'C' ";
			}
			else if (table_name == "TQMTS0M")
			{
				sqlstr = sqlstr + " AND IC_CC_FLAG = 'I' ";
			}

			sqlstr += "    AND base_code = @base_code ";
			sqlstr = sqlstr + " ORDER BY ST_NO ASC ";

			Log::Trace("", "", "sqlstr[{0}]", sqlstr);
			break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.ExecuteReader();
		fetchRowCount = 0;
		while (cmd_inq.Read())
		{
			tqmts0x["ST_NO"] = cmd_inq.GetString(1);
			tqmts0x["FACTORY_DIV"] = cmd_inq.GetString(2);
			tqmts0x["BASE_CODE"] = cmd_inq.GetString(3);

			fetchRowCount++;

			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[fetchRowCount - 1]["st_no"] = tqmts0x["ST_NO"];
			bcls_ret->Tables[0].Rows[fetchRowCount - 1]["base_code"] = tqmts0x["BASE_CODE"];
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT COUNT(1) "
					"  FROM TQMTS01 "
					" WHERE ST_NO = @st_no "
					"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
					"  AND  FACTORY_DIV =@factory_div"
					"  AND  BASE_CODE =@base_code"
				;
				break;
			}
			cmd_inq_01.SetCommandText(sqlstr);
			cmd_inq_01.Parameters.Set("st_no", tqmts0x["ST_NO"].ToString());
			cmd_inq_01.Parameters.Set("whole_backlog_code", s_whole_backlog_code);
			cmd_inq_01.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString().TrimOrBlank());
			cmd_inq_01.Parameters.Set("base_code", tqmts0x["BASE_CODE"].ToString().TrimOrBlank());

			v_count = cmd_inq_01.ExecuteScalar();
			if (v_count == 0)
			{
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["std_flag"] = "0";
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["factory_div"] = tqmts0x["FACTORY_DIV"];
			}
			else
			{
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["std_flag"] = v_count.ToString();
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["factory_div"] = tqmts0x["FACTORY_DIV"];

			}
			cmd_inq_01.Close();
		}
		cmd_inq.Close();
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", "","error=[{0}]", (const char*)str );

		strncpy(s.sysmsg, (const char*)str, 399);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
