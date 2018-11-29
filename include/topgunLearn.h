typedef enum learn_mode{  //!< learn_modeの切替
	LEARN_MODE_OFF    = 0,  //!< OFF
	LEARN_MODE_SOCRATES = 1,  //!< socrates準拠
	LEARN_MODE_ICHIHARA = 2,  //!< 市原先生論文レベル
	LEARN_MODE_KAJIHARA = 3,  //!< 梶原先生モード
	LEARN_MODE_KAJIHARA_D = 4,  //!< 梶原先生モード+with delete
	LEARN_MODE_DAG        = 5,  //!< DAG
	LEARN_MODE_DAG_D      = 6,  //!< DAG + with delete
} LEARN_MODE;
