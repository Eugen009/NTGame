#ifndef __E_MATERIAL_H__
#define __E_MATERIAL_H__

class EMaterial {
public:
	EMaterial();
	void Destroy();
	void SetVertexShader( const wchar_t* filename );
	void SetPixelShader( const wchar_t* filename );

	void PrepareMaterial();
	bool SetMaterial(ID3D11Device* dev, ID3D11DeviceContext* context);

protected:
	void CreateVertexShader(ID3D11Device* dev, const std::vector<byte>& fileData);
	void CreatePixelShader(ID3D11Device* dev, const std::vector<byte>& fileData);

protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;

	std::wstring m_strVertShader;
	std::wstring m_strPixelShader;

	bool m_bLoadCompleted;
	int m_iVexStride;

};

#endif