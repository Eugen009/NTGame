#ifndef __E_MATERIAL_H__
#define __E_MATERIAL_H__

class EMaterial {
public:
	EMaterial();
	void Destroy();
	void SetVertexShader( const wchar_t* filename );
	void SetPixelShader( const wchar_t* filename );
	void SetTexture( const wchar_t* filename );

	void Prepare();
	bool SetMaterial(ID3D11DeviceContext* context, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer);
	bool IsOK();
	bool CheckLoaded(ID3D11Device* dev);

protected:
	void CreateVertexShader(ID3D11Device* dev, const std::vector<byte>& fileData);
	void CreatePixelShader(ID3D11Device* dev, const std::vector<byte>& fileData);

protected:


	std::wstring m_strVertShader;
	std::wstring m_strPixelShader;
	std::wstring m_strTexture;

	bool m_bLoadCompleted;
	int m_iVexStride;

public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TexView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_Sampler;

};

#endif