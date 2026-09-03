/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-26
Description: 获取工艺卡精炼路径并自动生成TQMTS01表
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中




/*<remark>=========================================================
/// <summary>
/// 工艺卡确认中新增工序制造标准
/// <para>
/// 获取输入参数：TQMTS01(工序制造标准表) ；
/// </para>
/// <para>数据库表：TQMTS01(工序制造标准表)); 
///  前台QMTS01画面的F8(获取精炼路径)调用    </para>
/// </summary>
/// <param name="TQMTS01">工序制造标准表    </param>
===========================================================</remark>*/  

// service入口
BM2F_ENTERACE(qmts01_catch)

int f_qmts01_catch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	CString st_no = "";
	CString factory_div = "";
	CString base_code = "";
	CString whole_backlog_code= "";
	int doFlag = 0;
	int i;
	CDecimal i_count = 0;
	
	/* 实体类定义 */
	CModel tqmts01("TQMTS01");
	CModel tqmts0x("TQMTS0X");
	CModel tqmts02("TQMTS02");

	CModel tep0002("TEP0002");
	CModel tqmts03("TQMTS03");
	CModel tqmts04("TQMTS04");
	CModel tqmts05("TQMTS05");
	CModel tqmts06("TQMTS06");
	CModel tqmts07("TQMTS07");
	CModel tqmts08("TQMTS08");
	CModel tqmts0a("TQMTS0A");
	CModel tqmts0m("TQMTS0M");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_B(conn);
	CDbCommand cmd_inq_E(conn);
	CDbCommand cmd_inq_C(conn);
	CDbCommand cmd_inq_I(conn);
	try
	{ 
		st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().Trim();
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();
		tqmts01["ST_NO"] = st_no;
		tqmts01["FACTORY_DIV"] = factory_div;
		tqmts01["BASE_CODE"] = base_code;

		Log::Trace("", "", "qmts01_catch IN: st_no[{0}]factory_div[{1}]base_code[{2}]", (const char*)st_no, (const char*)factory_div, (const char*)base_code);
		//取得单行传入信息
		tqmts01.Reset();
		tqmts01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if(tqmts01["ST_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tqmts0x["ST_NO"] = tqmts01["ST_NO"];
		tqmts0x["FACTORY_DIV"] = tqmts01["FACTORY_DIV"];
		tqmts0x["BASE_CODE"] = base_code;
		tqmts0x.Query("ST_NO,FACTORY_DIV,BASE_CODE");
		//赋初值
		tqmts01["REC_CREATOR"] = s.userid;
	    tqmts01["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tqmts01["REC_REVISOR"] = " ";
		tqmts01["REC_REVISE_TIME"] = " ";
		tqmts01["ARCHIVE_FLAG"] = " ";
		tqmts01["IF_PLAN"] = "1";
		tqmts01["IF_PASS"] = "0";
		tqmts01["IF_MESSAGE"] = "0";
		tqmts01["MESSAGE_TIME"] = " ";
		tqmts01["FIN_CONFM_MAKER"] = " ";
		tqmts01["FIN_CONFM_TIME"] = " ";
		tqmts01["RES_CODE"] = " ";

		//tqmts01.Delete("FACTORY_DIV,ST_NO,BASE_CODE"); //条件字段项
		//tqmts01["WHOLE_BACKLOG_CODE"] = "";
		//tqmts01.TrimOrBlank();
		//tqmts01.Insert();
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
						 "  FROM TQMTS0X "
						 " WHERE ST_NO = @st_no "
						 " AND FACTORY_DIV=@factory_div"
					     " AND base_code = @base_code ";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", st_no);
		cmd_inq.Parameters.Set("factory_div", factory_div);
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts0x);
		}
		cmd_inq.Close();

		//电炉\转炉
		Log::Trace("", "","SMELT_DIV = [{0}]", (const char*)tqmts0x["SMELT_DIV"].ToString());
		if (tqmts0x["SMELT_DIV"].ToString().TrimOrBlank() == "B")
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT COUNT(1)"
					"  FROM TQMTS01 "
					" WHERE ST_NO = @st_no "
					" AND FACTORY_DIV=@factory_div"
					" AND base_code = @base_code "
					" AND WHOLE_BACKLOG_CODE = 'B'";
				break;
			}
			cmd_inq_B.SetCommandText(sqlstr);
			cmd_inq_B.Parameters.Set("st_no", st_no);
			cmd_inq_B.Parameters.Set("factory_div", factory_div);
			cmd_inq_B.Parameters.Set("base_code", base_code);
			i_count = cmd_inq_B.ExecuteScalar();
			cmd_inq_B.Close();
			if (i_count == 0)
			{
				tqmts01["WHOLE_BACKLOG_CODE"] = tqmts0x["SMELT_DIV"];
				//tqmts01["WHOLE_BACKLOG_SEQ"] = 1;
				tqmts01.TrimOrBlank();
				tqmts01.Insert();
				//为制造标准增加表
				tqmts04["REC_CREATOR"] = s.userid;
				tqmts04["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts04["REC_REVISOR"] = " ";
				tqmts04["REC_REVISE_TIME"] = " ";
				tqmts04["ARCHIVE_FLAG"] = " ";
				tqmts04["VERSION"] = 1;
				tqmts04["DU_FLAG"] = " ";
				tqmts04["DU_MAKER"] = " ";
				tqmts04["DU_TIME"] = " ";
				tqmts04["ST_NO"] = st_no;
				tqmts04["FACTORY_DIV"] = factory_div;
				tqmts04["BASE_CODE"] = base_code;
				tqmts04.TrimOrBlank();
				tqmts04.Insert();
			}
		}

        if (tqmts0x["SMELT_DIV"].ToString().TrimOrBlank() == "E")
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT COUNT(1)"
					"  FROM TQMTS01 "
					" WHERE ST_NO = @st_no "
					" AND FACTORY_DIV=@factory_div"
					" AND base_code = @base_code "
					" AND WHOLE_BACKLOG_CODE = 'E'";
				break;
			}
			cmd_inq_E.SetCommandText(sqlstr);
			cmd_inq_E.Parameters.Set("st_no", st_no);
			cmd_inq_E.Parameters.Set("factory_div", factory_div);
			cmd_inq_E.Parameters.Set("base_code", base_code);
			i_count = cmd_inq_E.ExecuteScalar();
			cmd_inq_E.Close();
			if (i_count == 0)
			{
				tqmts01["WHOLE_BACKLOG_CODE"] = tqmts0x["SMELT_DIV"];
				//tqmts01["WHOLE_BACKLOG_SEQ"] = 1;
				tqmts01.TrimOrBlank();
				tqmts01.Insert();
				//为制造标准增加表
				tqmts0a["REC_CREATOR"] = s.userid;
				tqmts0a["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts0a["REC_REVISOR"] = " ";
				tqmts0a["REC_REVISE_TIME"] = " ";
				tqmts0a["ARCHIVE_FLAG"] = " ";
				tqmts0a["VERSION"] = 1;
				tqmts0a["DU_FLAG"] = " ";
				tqmts0a["DU_MAKER"] = " ";
				tqmts0a["DU_TIME"] = " ";
				tqmts0a["ST_NO"] = st_no;
				tqmts0a["FACTORY_DIV"] = factory_div;
				tqmts0a["BASE_CODE"] = base_code;
				tqmts0a.TrimOrBlank();
				tqmts0a.Insert();
			}
		}

		//精炼
		Log::Trace("", "","REFINE_ROUTE_CODE = [{0}], lenth = [{1}]", (const char*)tqmts0x["REFINE_ROUTE_CODE"].ToString(),tqmts0x["REFINE_ROUTE_CODE"].ToString().GetLength());
		if(tqmts0x["REFINE_ROUTE_CODE"].ToString().TrimOrBlank() != "0000")  //ad by FXY 精炼路径无规定时，不做
		{
			for(i=0; i<tqmts0x["REFINE_ROUTE_CODE"].ToString().GetLength(); i++)
			{
				//HYF 20130427 SubstringNE
				tqmts01["WHOLE_BACKLOG_CODE"] = tqmts0x["REFINE_ROUTE_CODE"].ToString().SubstringNE(i,1);
				Log::Trace("", "","tqmts01.WHOLE_BACKLOG_CODE= [{0}]", (const char*)tqmts01["WHOLE_BACKLOG_CODE"].ToString());
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT COUNT(1) "
								 "  FROM TQMTS01 "
								 " WHERE   "
								 "   ST_NO = @st_no "
								 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
								 " AND FACTORY_DIV=@factory_div"
						         " AND BASE_CODE =@base_code";
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("st_no", st_no);
				cmd_inq.Parameters.Set("whole_backlog_code", tqmts01["WHOLE_BACKLOG_CODE"].ToString());
				cmd_inq.Parameters.Set("base_code", base_code);
				i_count = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
				if(i_count == 0)
				{
				//	tqmts01["WHOLE_BACKLOG_SEQ"] = tqmts01["WHOLE_BACKLOG_SEQ"].ToDecimal() + 1;
					tqmts01.TrimOrBlank();
					tqmts01.Insert();
					//为制造标准增加表
					if (tqmts01["WHOLE_BACKLOG_CODE"].ToString().TrimOrBlank() == "L")
					{
						tqmts07["REC_CREATOR"] = s.userid;
						tqmts07["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
						tqmts07["REC_REVISOR"] = " ";
						tqmts07["REC_REVISE_TIME"] = " ";
						tqmts07["ARCHIVE_FLAG"] = " ";
						tqmts07["VERSION"] = 1;
						tqmts07["DU_FLAG"] = " ";
						tqmts07["DU_MAKER"] = " ";
						tqmts07["DU_TIME"] = " ";
						tqmts07["ST_NO"] = st_no;
						tqmts07["FACTORY_DIV"] = factory_div;
						tqmts07["BASE_CODE"] = base_code;
						tqmts07.TrimOrBlank();
						tqmts07.Insert();
					}
					if (tqmts01["WHOLE_BACKLOG_CODE"].ToString().TrimOrBlank() == "R")
					{
						tqmts06["REC_CREATOR"] = s.userid;
						tqmts06["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
						tqmts06["REC_REVISOR"] = " ";
						tqmts06["REC_REVISE_TIME"] = " ";
						tqmts06["ARCHIVE_FLAG"] = " ";
						tqmts06["VERSION"] = 1;
						tqmts06["DU_FLAG"] = " ";
						tqmts06["DU_MAKER"] = " ";
						tqmts06["DU_TIME"] = " ";
						tqmts06["ST_NO"] = st_no;
						tqmts06["FACTORY_DIV"] = factory_div;
						tqmts06["BASE_CODE"] = base_code;
						tqmts06.TrimOrBlank();
						tqmts06.Insert();
					}
					if (tqmts01["WHOLE_BACKLOG_CODE"].ToString().TrimOrBlank() == "V")
					{
						tqmts05["REC_CREATOR"] = s.userid;
						tqmts05["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
						tqmts05["REC_REVISOR"] = " ";
						tqmts05["REC_REVISE_TIME"] = " ";
						tqmts05["ARCHIVE_FLAG"] = " ";
						tqmts05["VERSION"] = 1;
						tqmts05["DU_FLAG"] = " ";
						tqmts05["DU_MAKER"] = " ";
						tqmts05["DU_TIME"] = " ";
						tqmts05["ST_NO"] = st_no;
						tqmts05["FACTORY_DIV"] = factory_div;
						tqmts05["BASE_CODE"] = base_code;
						tqmts05.TrimOrBlank();
						tqmts05.Insert();
					}
				}
			}
		}
		//模连铸
		Log::Trace("", "", "IC_CC_FLAG = [{0}]", (const char*)tqmts0x["IC_CC_FLAG"].ToString());
		if (tqmts0x["IC_CC_FLAG"].ToString().TrimOrBlank() == "C")
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT COUNT(1)"
					"  FROM TQMTS01 "
					" WHERE ST_NO = @st_no "
					" AND FACTORY_DIV=@factory_div"
					" AND base_code = @base_code "
					" AND WHOLE_BACKLOG_CODE = 'C'";
				break;
			}
			cmd_inq_C.SetCommandText(sqlstr);
			cmd_inq_C.Parameters.Set("st_no", st_no);
			cmd_inq_C.Parameters.Set("factory_div", factory_div);
			cmd_inq_C.Parameters.Set("base_code", base_code);
			i_count = cmd_inq_C.ExecuteScalar();
			cmd_inq_C.Close();
			if (i_count == 0)
			{
				tqmts01["WHOLE_BACKLOG_CODE"] = tqmts0x["IC_CC_FLAG"];
				//tqmts01["WHOLE_BACKLOG_SEQ"] = 1;
				tqmts01.TrimOrBlank();
				tqmts01.Insert();

				tqmts08["REC_CREATOR"] = s.userid;
				tqmts08["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts08["REC_REVISOR"] = " ";
				tqmts08["REC_REVISE_TIME"] = " ";
				tqmts08["ARCHIVE_FLAG"] = " ";
				tqmts08["VERSION"] = 1;
				tqmts08["DU_FLAG"] = " ";
				tqmts08["DU_MAKER"] = " ";
				tqmts08["DU_TIME"] = " ";
				tqmts08["ST_NO"] = st_no;
				tqmts08["FACTORY_DIV"] = factory_div;
				tqmts08["BASE_CODE"] = base_code;
				tqmts08.TrimOrBlank();
				tqmts08.Insert();
			}
		}
		if (tqmts0x["IC_CC_FLAG"].ToString().TrimOrBlank() == "I")
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT COUNT(1)"
					"  FROM TQMTS01 "
					" WHERE ST_NO = @st_no "
					" AND FACTORY_DIV=@factory_div"
					" AND base_code = @base_code "
					" AND WHOLE_BACKLOG_CODE = 'I'";
				break;
			}
			cmd_inq_I.SetCommandText(sqlstr);
			cmd_inq_I.Parameters.Set("st_no", st_no);
			cmd_inq_I.Parameters.Set("factory_div", factory_div);
			cmd_inq_I.Parameters.Set("base_code", base_code);
			i_count = cmd_inq_I.ExecuteScalar();
			cmd_inq_I.Close();
			if (i_count == 0)
			{
				tqmts01["WHOLE_BACKLOG_CODE"] = tqmts0x["IC_CC_FLAG"];
				//tqmts01["WHOLE_BACKLOG_SEQ"] = 1;
				tqmts01.TrimOrBlank();
				tqmts01.Insert();

				tqmts0m["REC_CREATOR"] = s.userid;
				tqmts0m["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts0m["REC_REVISOR"] = " ";
				tqmts0m["REC_REVISE_TIME"] = " ";
				tqmts0m["ARCHIVE_FLAG"] = " ";
				tqmts0m["VERSION"] = 1;
				tqmts0m["DU_FLAG"] = " ";
				tqmts0m["DU_MAKER"] = " ";
				tqmts0m["DU_TIME"] = " ";
				tqmts0m["ST_NO"] = st_no;
				tqmts0m["FACTORY_DIV"] = factory_div;
				tqmts0m["BASE_CODE"] = base_code;
				tqmts0m.TrimOrBlank();
				tqmts0m.Insert();
			}
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
