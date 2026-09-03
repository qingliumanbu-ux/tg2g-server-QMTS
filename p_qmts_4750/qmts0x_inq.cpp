//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中




/*<remark>=========================================================
/// <summary>
/// 查询工艺卡
/// <para>
/// 1.查询工艺卡和对应的成分标准信息
/// </para>
/// <para>数据库表：TQMTS0X(工艺卡表);
//                  TQMTS02(工序成分标准表)                </para>
/// <para>主调用函数：前台QMTS0X画面的F2(查询)调用。       </para>
/// </summary>
/// <param name="ST_NO"> 出钢记号                          </param>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts0x_inq)

int f_qmts0x_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CDecimal q_flag = 0;
	CString q_factory_div(" ");
	CString q_base_code(" ");
	CString q_st_no("");
	CString check_maker(" ");
	CString valid_flag("");

	CString ic_cc_flag("");
	CString smelt_div(" ");

	CString remark(" ");
	CString check_time_from("");
	CString check_time_to("");

	CString q_sg_sign;
	CString form_type;
	/* 实体类定义 */
	CModel tqmts0x("TQMTS0X");
	CModel tqmts02("TQMTS02");
	CModel tep0002("TEP0002");

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_tep0002(conn);

	try
	{
		Log::Trace("","","进入后台查询qmts0x_inq");
		/*获得传入参数*/
		q_flag = bcls_rec->Tables[1].Rows[0]["flag"].ToDecimal();
		q_base_code = bcls_rec->Tables[1].Rows[0]["base_code"].ToString().Trim();
		q_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		q_factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().Trim();
		 
		Log::Trace("", "", "断点1");
		Log::Trace("", "", "qmts0x_inq IN:---FLAG = [{0}]", q_flag.ToInt32());
		Log::Trace("", "", "qmts0x_inq IN:---ST_NO = [{0}]", (const char*)q_st_no);
		Log::Trace("", "", "qmts0x_ins IN:---BASE_CODE = [{0}]",q_base_code.Trim());
		Log::Trace("", "", "qmts0x_inq IN:---q_factory_div = [{0}]", (const char*)q_factory_div);
		//厂别区分不再写死，前台如果传空，则表示不区分厂别，如果传了值，按前台的设定赋值
		//q_factory_div = "A";

		if (q_flag == 0)
		{
			check_maker = bcls_rec->Tables[0].Rows[0]["check_maker"].ToString().Trim();
			valid_flag = bcls_rec->Tables[0].Rows[0]["valid_flag"].ToString().Trim();
			remark = bcls_rec->Tables[0].Rows[0]["remark"].ToString().Trim();

			ic_cc_flag = bcls_rec->Tables[0].Rows[0]["ic_cc_flag"].ToString().Trim();
			smelt_div = bcls_rec->Tables[0].Rows[0]["smelt_div"].ToString().Trim();
			check_time_to = bcls_rec->Tables[0].Rows[0]["check_time_to"].ToString().Trim();
			check_time_from = bcls_rec->Tables[0].Rows[0]["check_time_from"].ToString().Trim();
			//check_time = bcls_rec->Tables[0].Rows[0]["check_time"].ToString().Trim();
			if (check_time_from == "")
			{
				check_time_from = " ";
			}
			if (check_time_to == "")
			{
				check_time_to = " ";
			}
		//	check_time1 = bcls_rec->Tables[0].Rows[0]["check_time1"].ToString().Trim();
			q_sg_sign = bcls_rec->Tables[0].Rows[0]["sg_sign"].ToString().Trim();
			form_type = bcls_rec->Tables[1].Rows[0]["form_type"].ToString().Trim(); //form_type用于判断调用该服务的画面update by LiYongle 20170907

			Log::Trace("", "", "qmts0x_inq IN:---SG_SIGN = [{0}]", (const char*)q_sg_sign);

	//		sqlstr = " SELECT DISTINCT ST_NO, VALID_FLAG,FACTORY_DIV "
			sqlstr = " SELECT * "
				"   FROM TQMTS0X "
				"  WHERE 1=1 ";
				if (q_factory_div != "")
				{
					sqlstr += " AND FACTORY_DIV like '%" + q_factory_div + "%'";
				}
				if (q_st_no != "")
				{
					sqlstr += "    AND ST_NO LIKE '%" + q_st_no + "%' ";
				}
				if (check_maker != "")
				{
					sqlstr += "    AND CHECK_MAKER LIKE '%" + check_maker + "%' ";
				}
				if (valid_flag != "")
				{
					sqlstr += "    AND VALID_FLAG  LIKE '%" + valid_flag + "%' ";
				}
				if (remark != "")
				{
					sqlstr += "    AND REMARK LIKE '%" + remark + "%' ";
				}
				if (check_time_from != "")
				{
					sqlstr += "    AND SUBSTR(CHECK_TIME,0,8)  >= '" + check_time_from + "'";
				}
				if (check_time_to != "")
				{
					sqlstr += " AND SUBSTR(CHECK_TIME, 0, 8) <= '" + check_time_to + "' ";
				}
				if (q_sg_sign != "")
				{
					sqlstr += "    AND (LABEL1 LIKE '%" + q_sg_sign + "%' "
						"    OR LABEL2 LIKE '%" + q_sg_sign + "%' "
						"    OR LABEL3 LIKE '%" + q_sg_sign + "%' "
						"    OR LABEL4 LIKE '%" + q_sg_sign + "%' "
						"    OR LABEL5 LIKE '%" + q_sg_sign + "%' ) ";
				}
			//	sqlstr += "    AND base_code = @q_base_code ";
			Log::Trace("", "", "form_type[{0}]: ", form_type);
			Log::Trace("", "", "测试1: SQL[{0}]: ", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("q_base_code", q_base_code);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		else
		{
			if (q_st_no.Trim() == "")
			{
				strcpy(s.msg, _RES("QM00S0004171")/*出钢记号不可为空。*/);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//获取工艺卡标准
			//增加块名QMTSBLK01
			bcls_ret->Tables.Add("QMTSBLK01");
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
					"   FROM TQMTS0X "
					"  WHERE FACTORY_DIV like '"+q_factory_div +"%'"
					"    AND ST_NO = '"+q_st_no +"'"
					"    AND base_code = @q_base_code ";
				
				break;
			}
			Log::Trace("", "", "测试3: SQL[{0}]: ", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
		//	cmd_inq.Parameters.Set("q_factory_div", q_factory_div);
		//	cmd_inq.Parameters.Set("q_st_no", q_st_no);
			cmd_inq.Parameters.Set("q_base_code", q_base_code);
			cmd_inq.ExecuteQuery(bcls_ret->Tables["QMTSBLK01"]);
			////////////////挂索引模式只显示实际成分标准，没有则不显示update by yiling 20160412
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT IDX_NO_ELM "
					"   FROM TQMTS0X "
					"  WHERE ST_NO = '"+q_st_no+"' "
					" AND FACTORY_DIV = '"+q_factory_div+"' "
					" AND base_code = @q_base_code ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
		//	cmd_inq.Parameters.Set("tqmts0x.FACTORY_DIV", q_factory_div);
		//	cmd_inq.Parameters.Set("tqmts0x.ST_NO", q_st_no);
			cmd_inq.Parameters.Set("q_base_code", q_base_code);
			cmd_inq.ExecuteReader();
			Log::Trace("", "", "测试2: SQL[{0}]: ", sqlstr);
			if (cmd_inq.Read())
			{
				tqmts0x["IDX_NO_ELM"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			Log::Trace("", "", "1111tqmts0x.IDX_NO_ELM= [{0}]", (const char*)tqmts0x["IDX_NO_ELM"].ToString());
			//获取成分标准
			//增加块名QMTSBLK02
			bcls_ret->Tables.Add("QMTSBLK02");
			if (tqmts0x["IDX_NO_ELM"].ToString().Trim() == "")
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
						"   FROM TEP0002 "
						"  WHERE TRIM(CODE_CLASS) = 'QMYS' "
						"    AND TRIM(CODE_DESC_2_CONTENT) IS NOT null "
						//"    AND CODE != 'A06' "
						//"    AND CODE != 'A07' "
						//"    AND CODE != 'A08' "
						//"    AND CODE != 'A09' "
						//"    AND CODE != 'A10' "
						//"    AND CODE != 'A11' "
						"  ORDER BY CODE_DESC_2_CONTENT ASC ";
					break;
				}
				cmd_tep0002.SetCommandText(sqlstr);
				cmd_tep0002.ExecuteReader();
				while (cmd_tep0002.Read())
				{
					cmd_tep0002.Fetch(tep0002);

					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = " SELECT * "
							"   FROM TQMTS02 "
							"  WHERE FACTORY_DIV = @q_factory_div "
							"    AND WHOLE_BACKLOG_CODE = 'G' "
							"    AND ST_NO = @q_st_no "
							"    AND ELM_CODE = @tep0002.CODE "
							"    AND base_code = @q_base_code ";
						break;
					}
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("q_factory_div", q_factory_div);
					cmd_inq.Parameters.Set("q_st_no", q_st_no);
					cmd_inq.Parameters.Set("q_base_code", q_base_code);
					cmd_inq.Parameters.Set("tep0002.CODE", tep0002["CODE"].ToString());
					cmd_inq.ExecuteReader();

					if (cmd_inq.Read())
					{
						cmd_inq.Fetch(tqmts02);
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
					cmd_inq.Close();
					if (tqmts02["ELM_ACCU"].ToDecimal() != 0)
					{
						tqmts02["MAIN_MIN"] = tqmts02["MAIN_MIN"].ToDecimal().Round(tqmts02["ELM_ACCU"].ToDecimal().ToInt32());
						tqmts02["MAIN_MAX"] = tqmts02["MAIN_MAX"].ToDecimal().Round(tqmts02["ELM_ACCU"].ToDecimal().ToInt32());
						tqmts02["MAIN_AIM"] = tqmts02["MAIN_AIM"].ToDecimal().Round(tqmts02["ELM_ACCU"].ToDecimal().ToInt32());
						tqmts02["SPE_MIN"] = tqmts02["SPE_MIN"].ToDecimal().Round(tqmts02["ELM_ACCU"].ToDecimal().ToInt32());
						tqmts02["SPE_MAX"] = tqmts02["SPE_MAX"].ToDecimal().Round(tqmts02["ELM_ACCU"].ToDecimal().ToInt32());
					}
					tqmts02.MergeTo(bcls_ret->Tables["QMTSBLK02"], false);
				}
				cmd_tep0002.Close();
			}
			else
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
						"   FROM TQMTS02 "
						"  WHERE FACTORY_DIV = @q_factory_div "
						"    AND ST_NO = @q_st_no "
						"    AND base_code = @q_base_code ";

					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("q_factory_div", q_factory_div);
				cmd_inq.Parameters.Set("q_st_no", q_st_no);
				cmd_inq.Parameters.Set("q_base_code", q_base_code);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					cmd_inq.Fetch(tqmts02);
					tqmts02.MergeTo(bcls_ret->Tables["QMTSBLK02"], false);
				}
				cmd_inq.Close();
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
