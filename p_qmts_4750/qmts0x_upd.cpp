/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-11
Description: 修改工艺卡
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中






/*<remark>=========================================================
/// <summary>
/// 修改工艺卡
/// <para>
/// 1.修改工艺卡和对应的成分标准信息
/// </para>
/// <para>数据库表：TQMTS0X(工艺卡表);
//                  TQMTS02(工序成分标准表)                </para>
/// <para>主调用函数：前台QMTS0X画面的F4(修改)调用。       </para>
/// </summary>
/// <param name="ST_NO"> 出钢记号                          </param>
===========================================================</remark>*/

/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用

// service入口
BM2F_ENTERACE(qmts0x_upd)

int f_qmts0x_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	CString base_code = " ";

	CDecimal flag = 0;
	CDecimal i_count = 0;

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[1].Columns.Add(DT_STRING,"ST_IDX_NO");

	/* 实体类定义 */
	CModel tqmts0x("TQMTS0X");
	CModel hqmts0x("HQMTS0X");
	CModel tqmts02("TQMTS02");
	CModel hqmts02("HQMTS02");
	CModel tep0002("TEP0002");

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/*获得传入参数*/
		flag = bcls_rec->Tables[0].Rows[0]["FLAG"].ToDecimal();
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();
		tqmts0x["BASE_CODE"] = base_code;
		tqmts02["BASE_CODE"] = base_code;
		tqmts0x["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
//		tqmts0x["VALID_FLAG"] = bcls_rec->Tables[0].Rows[0]["VALID_FLAG"].ToString().Trim();
		/////字符型DB2数据库是个null, oracle是个空格，update by yiling 20160620,主键不好改成动态SQL查询
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容	
			tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
			break;
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
			break;
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank();
			break;
		}
		Log::Trace("", "","qmts0x_upd IN:---FLAG = [{0}]",flag.ToInt32());
		Log::Trace("", "","qmts0x_upd IN:---ST_NO = [{0}]",(const char*)tqmts0x["ST_NO"].ToString());
		Log::Trace("", "", "qmts0x_ins IN:---BASE_CODE = [{0}]", base_code.Trim());
		Log::Trace("", "", "qmts0x_ins IN:---FACTORY_DIV = [{0}]", (const char*)tqmts0x["FACTORY_DIV"].ToString());
//		Log::Trace("", "", "qmts0x_ins IN:---VALID_FLAG = [{0}]", (const char*)tqmts0x["VALID_FLAG"].ToString());
		//厂别区分不再写死，前台如果传空，则表示不区分厂别，如果传了值，按前台的设定赋值
		//tqmts0x["FACTORY_DIV"] = "A";

		//校验出钢记号是否已审核——add by 冯晓轶 2012-04-26
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT VALID_FLAG "
				"   FROM TQMTS0X "						   
				"  WHERE FACTORY_DIV = @tqmts0x.FACTORY_DIV "
				"    AND ST_NO = @tqmts0x.ST_NO "
				"    AND base_code = @base_code ";

			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("base_code",base_code);
		cmd_inq.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			tqmts0x["VALID_FLAG"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		if(tqmts0x["VALID_FLAG"].ToString() == "1")
		{
			strcpy(s.msg,_RES("QM00S0005918")/*该出钢记号已审核，不可修改。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//调用检查出钢记号是否在计划中使用的函数——add by 冯晓轶 2012-03-23
		bcls_rec_s.Tables[0].Rows.Add();
		bcls_rec_s.Tables[0].Rows[0]["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
		bcls_rec_s.Tables[1].Rows.Add();
		bcls_rec_s.Tables[1].Rows[0]["ST_IDX_NO"] = tqmts0x["ST_NO"];

		//doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret,conn);

		if(doFlag != 0)
		{
			Log::Trace("", "","f_pssm_stno_chk_using REEOR");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if(flag == 0)		//修改工艺卡
		{
			//写工艺卡历史表——add by 冯晓轶 2012-03-23
			Log::Trace("", "","INSERT HQMTS0X");
			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
					"   FROM TQMTS0X "						   
					"  WHERE FACTORY_DIV = @tqmts0x.FACTORY_DIV "
					"    AND ST_NO = @tqmts0x.ST_NO "
					"    AND base_code = @base_code ";
				
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
			cmd_inq.Parameters.Set("base_code", base_code);
			cmd_inq.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				cmd_inq.Fetch(hqmts0x);
			}
			cmd_inq.Close();
			hqmts0x["DU_FLAG"] = "U";
			hqmts0x["DU_MAKER"] = s.userid;
			hqmts0x["DU_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			hqmts0x.TrimOrBlank();
			hqmts0x.Insert();

			//删除工艺卡表
			Log::Trace("", "","DELETE TQMTS0X");

			tqmts0x.Delete("FACTORY_DIV,ST_NO,BASE_CODE");

			//新增工艺卡内容
			Log::Trace("", "","INSERT TQMTS0X");
			tqmts0x.Reset();
			tqmts0x.MergeFrom(bcls_rec->Tables[0].Rows[0]);

						if(tqmts0x["PUR_TEMP_MIN"].ToDecimal() > tqmts0x["PUR_TEMP_MAX"].ToDecimal())
			{
					strcpy(s.msg,"浇铸温度下限不能大于浇铸温度上限。");
					throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts0x["INGOT_PUR_SPEED_MIN"].ToDecimal() > tqmts0x["INGOT_PUR_SPEED_MAX"].ToDecimal())
			{
					strcpy(s.msg,"锭身浇铸速度下限不能大于锭身浇铸速度上限。");
					throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts0x["INGOT_PUR_SPEED_2_MIN"].ToDecimal() > tqmts0x["INGOT_PUR_SPEED_2_MAX"].ToDecimal())
			{
					strcpy(s.msg,"第2锭盘浇铸速度下限不能大于第2锭盘浇铸速度上限。");
					throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts0x["CAP_PUR_SPEED_MIN"].ToDecimal() > tqmts0x["CAP_PUR_SPEED_MAX"].ToDecimal())
			{
					strcpy(s.msg,"帽头浇铸速度下限不能大于帽头浇铸速度上限。");
					throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts0x["LABEL1_L"].ToDecimal() > tqmts0x["LABEL1_U"].ToDecimal())
			{
					strcpy(s.msg,"适用钢号1下限不能大于适用钢号1上限。");
					throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts0x["LABEL2_L"].ToDecimal() > tqmts0x["LABEL2_U"].ToDecimal())
			{
					strcpy(s.msg,"适用钢号2下限不能大于适用钢号2上限。");
					throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts0x["LABEL3_L"].ToDecimal() > tqmts0x["LABEL3_U"].ToDecimal())
			{
					strcpy(s.msg,"适用钢号3下限不能大于适用钢号3上限。");
					throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts0x["LABEL4_L"].ToDecimal() > tqmts0x["LABEL4_U"].ToDecimal())
			{
					strcpy(s.msg,"适用钢号4下限不能大于适用钢号4上限。");
					throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts0x["LABEL5_L"].ToDecimal() > tqmts0x["LABEL5_U"].ToDecimal())
			{
					strcpy(s.msg,"适用钢号5下限不能大于适用钢号5上限。");
					throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts0x["CAST_SPEED_MIN"].ToDecimal() > tqmts0x["CAST_SPEED_MAX"].ToDecimal())
			{
					strcpy(s.msg,"浇铸速度最小值不能大于浇铸速度最大值。");
					throw CApplicationException(-1, s.msg, log.Location);
			}


			/******************** 赋初值 *********************/
			tqmts0x["REC_CREATOR"] = hqmts0x["REC_CREATOR"];
			tqmts0x["REC_CREATE_TIME"] = hqmts0x["REC_CREATE_TIME"];
			tqmts0x["REC_REVISOR"] = s.userid;
			tqmts0x["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts0x["VERSION"] = hqmts0x["VERSION"].ToDecimal() + 1;//修改一次，版本加1
			//tqmts0x["FACTORY_DIV"] = "A";

			tqmts0x["VALID_FLAG"] = "0";
			tqmts0x["CHECK_TIME"] = " ";
			tqmts0x["CHECK_MAKER"] = " ";

			tqmts0x.TrimOrBlank();
			tqmts0x.Insert();
		}
		else if(flag == 1)	//修改工艺卡成分
		{
			Log::Trace("", "", "qmts0x_upd IN:--修改工艺卡成分");
			//对输入信息循环处理
			for (i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++ )
			{
				//取得单行传入信息
				tqmts02.Reset();
				tqmts02.MergeFrom(bcls_rec->Tables[1].Rows[i]);

				if(tqmts02["ELM_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg,_RES("QM00S0004168")/*元素代码不能为空。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if(tqmts02["SMELT_CHEMI_FLAG"].ToString().Trim() == "")
				{
					strcpy(s.msg,_RES("QM00S0005925")/*元素指示不能为空。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/******************** 赋初值 *************************/
				tqmts02["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts02["ST_NO"] = tqmts0x["ST_NO"];
				tqmts02["WHOLE_BACKLOG_CODE"] = "G";
				tqmts02["WHOLE_BACKLOG_SEQ"] = 0;

				//数据效验——add by 冯晓轶 2012-03-23
				if(tqmts02["MAIN_MIN"].ToDecimal()  >tqmts02["MAIN_MAX"].ToDecimal() || tqmts02["MAIN_AIM"].ToDecimal() >tqmts02["MAIN_MAX"].ToDecimal() || tqmts02["MAIN_MIN"].ToDecimal() >tqmts02["MAIN_AIM"].ToDecimal())
				{
					CFormattable arguments[] = {(const char*)tqmts02["ST_NO"].ToString(),(const char*)tqmts02["WHOLE_BACKLOG_CODE"].ToString(),(const char*)tqmts02["ELM_NAME"].ToString()};
					CMessageFormat::Format(s.msg, _RES("QM00S0004178")/*出钢记号[{0}]工序[{1}]对应的[{2}]元素主试成分数据倒置。*/, arguments, 3);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if(tqmts02["SPE_MIN"].ToDecimal()  >tqmts02["SPE_MAX"].ToDecimal() )
				{
					CFormattable arguments[] = {(const char*)tqmts02["ST_NO"].ToString(),(const char*)tqmts02["WHOLE_BACKLOG_CODE"].ToString(),(const char*)tqmts02["ELM_NAME"].ToString()};
					CMessageFormat::Format(s.msg, _RES("QM00S0004001")/*出钢记号[{0}]工序[{1}]对应的[{2}]元素特采成分数据倒置。*/, arguments, 3);
					throw CApplicationException(-1, s.msg, log.Location);
				}	

				//根据成分标准信息是否不存在，确定是新增，还是修改。
				switch(conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT COUNT(1) "
						"   FROM TQMTS02 "
						"  WHERE FACTORY_DIV = @tqmts02.FACTORY_DIV "
						"    AND WHOLE_BACKLOG_CODE = @tqmts02.WHOLE_BACKLOG_CODE "
						"    AND ST_NO = @tqmts02.ST_NO "
						"    AND ELM_CODE = @tqmts02.ELM_CODE ";
						"    AND base_code = @base_code ";

					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("tqmts02.FACTORY_DIV", tqmts02["FACTORY_DIV"].ToString());
				cmd_inq.Parameters.Set("base_code", base_code);
				cmd_inq.Parameters.Set("tqmts02.WHOLE_BACKLOG_CODE", tqmts02["WHOLE_BACKLOG_CODE"].ToString());
				cmd_inq.Parameters.Set("tqmts02.ST_NO", tqmts02["ST_NO"].ToString());
				cmd_inq.Parameters.Set("tqmts02.ELM_CODE", tqmts02["ELM_CODE"].ToString());
				i_count = cmd_inq.ExecuteScalar();	//ExecuteScalar只返回查询结果集中的第一行的第一列，忽略额外的列或行，适用于单纯计算COUNT,SUM,MAX等的sql语句。
				cmd_inq.Close();
				if(i_count <= 0)	//成分信息不存在，先新增，再修改。
				{
					/******************** 赋初值 *************************/
					tqmts02["REC_CREATOR"] = s.userid;
					tqmts02["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tqmts02["REC_REVISOR"] = " ";
					tqmts02["REC_REVISE_TIME"] = " ";
					tqmts02["ARCHIVE_FLAG"] = " ";
					tqmts02["VERSION"] = 1;
					tqmts02["DU_FLAG"] = " ";
					tqmts02["DU_MAKER"] = " ";
					tqmts02["DU_TIME"] = " ";
                    tqmts02["BASE_CODE"] = base_code;


					//获取元素顺序、元素单位——add by 冯晓轶 2012-03-23
					switch(conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = " SELECT * "
							"   FROM TEP0002 "
							"  WHERE CODE_CLASS = 'QMYS' "
							"    AND CODE = @tqmts02.ELM_CODE ";
						break;
					}
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("tqmts02.ELM_CODE", tqmts02["ELM_CODE"].ToString());
					cmd_inq.ExecuteReader();
					if(cmd_inq.Read())
					{
						cmd_inq.Fetch(tep0002);
						tqmts02["ELM_POS"] = CDecimal::Parse(tep0002["CODE_DESC_2_CONTENT"].ToString());//得到元素顺序
						tqmts02["ELM_UNIT"] = "%";
					}
					cmd_inq.Close();

					if(tqmts02["MAIN_MAX"].ToDecimal() == 99.999 &&( tqmts02["ELM_CODE"].ToString() == "064" || tqmts02["ELM_CODE"].ToString() == "058"
						||tqmts02["ELM_CODE"].ToString() == "052" ||tqmts02["ELM_CODE"].ToString() == "096"
						||tqmts02["ELM_CODE"].ToString() == "051" ||tqmts02["ELM_CODE"].ToString() == "093"
						||tqmts02["ELM_CODE"].ToString() == "048" ||tqmts02["ELM_CODE"].ToString() == "011"
						||tqmts02["ELM_CODE"].ToString() == "075" ||tqmts02["ELM_CODE"].ToString() == "209"
						||tqmts02["ELM_CODE"].ToString() == "119" ||tqmts02["ELM_CODE"].ToString() == "091"
						||tqmts02["ELM_CODE"].ToString() == "207" ||tqmts02["ELM_CODE"].ToString() == "065"
						||tqmts02["ELM_CODE"].ToString() == "122" ))
					{
						tqmts02["MAIN_MAX"] = 0;
					}
					tqmts02.TrimOrBlank();
					tqmts02.Insert();	
				}
				else       //成分信息存在，直接修改。
				{
					//写成分标准历史表
					switch(conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = " SELECT * "
							"   FROM TQMTS02 "
							"  WHERE FACTORY_DIV = @tqmts02.FACTORY_DIV "
							"    AND WHOLE_BACKLOG_CODE = @tqmts02.WHOLE_BACKLOG_CODE "
							"    AND ST_NO = @tqmts02.ST_NO "
							"    AND ELM_CODE = @tqmts02.ELM_CODE ";
							"    AND base_code = @base_code ";
						break;
					}
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("tqmts02.FACTORY_DIV", tqmts02["FACTORY_DIV"].ToString());
					cmd_inq.Parameters.Set("base_code", base_code);
					cmd_inq.Parameters.Set("tqmts02.WHOLE_BACKLOG_CODE", tqmts02["WHOLE_BACKLOG_CODE"].ToString());
					cmd_inq.Parameters.Set("tqmts02.ST_NO", tqmts02["ST_NO"].ToString());
					cmd_inq.Parameters.Set("tqmts02.ELM_CODE", tqmts02["ELM_CODE"].ToString());
					cmd_inq.ExecuteReader();
					if(cmd_inq.Read())
					{
						cmd_inq.Fetch(hqmts02);
					}
					cmd_inq.Close();
					hqmts02["DU_FLAG"] = "U";
					hqmts02["DU_MAKER"] = s.userid;
					hqmts02["DU_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					hqmts02.TrimOrBlank();
					hqmts02.Insert();

					/******************** 赋初值 *************************/
					tqmts02["REC_REVISOR"] = s.userid;
					tqmts02["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tqmts02["VERSION"] = hqmts02["VERSION"].ToDecimal() + 1;
				    tqmts02["BASE_CODE"] = base_code;

					
					//修改成分标准内容
					tqmts02.Update("REC_REVISOR, REC_REVISE_TIME, VERSION, MAIN_MIN, MAIN_MAX, MAIN_AIM, SPE_MIN, SPE_MAX, SMELT_CHEMI_FLAG, ROUND_CODE, ELM_ACCU, BASE_CODE");
				}
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
