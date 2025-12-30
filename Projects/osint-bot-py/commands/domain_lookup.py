import aiohttp
import discord
from discord.ext import commands
import socket

class DomainLookup(commands.Cog):
    def __init__(self, bot):
        self.bot = bot

    @commands.command()
    async def domain(self, ctx, domain: str):
        """Analizza un dominio e mostra informazioni OSINT"""

        # Risolvi dominio → IP
        try:
            ip_address = socket.gethostbyname(domain)
        except socket.gaierror:
            await ctx.send("❌ Dominio non valido o impossibile da risolvere.")
            return

        # API IP-API per info sull'IP
        url = f"http://ip-api.com/json/{ip_address}?fields=66846719"

        async with aiohttp.ClientSession() as session:
            async with session.get(url) as response:
                data = await response.json()

        if data["status"] != "success":
            await ctx.send("❌ Impossibile ottenere informazioni sul dominio.")
            return

        # Embed
        embed = discord.Embed(
            title=f"🌐 Domain Lookup: {domain}",
            color=discord.Color.green()
        )

        embed.add_field(name="🔢 IP", value=ip_address, inline=True)
        embed.add_field(name="🌍 Paese", value=data.get("country", "N/A"), inline=True)
        embed.add_field(name="🏙️ Regione", value=data.get("regionName", "N/A"), inline=True)
        embed.add_field(name="📍 Città", value=data.get("city", "N/A"), inline=True)
        embed.add_field(name="🌐 ISP / Hosting", value=data.get("isp", "N/A"), inline=True)
        embed.add_field(name="🏢 ASN", value=data.get("as", "N/A"), inline=True)

        # DNS lookup
        try:
            dns_records = socket.getaddrinfo(domain, None)
            dns_list = list(set([record[4][0] for record in dns_records]))
            dns_text = "\n".join(dns_list)
        except:
            dns_text = "N/A"

        embed.add_field(name="📡 DNS Records", value=dns_text, inline=False)

        embed.set_footer(text="Powered by ip-api.com")

        await ctx.send(embed=embed)

async def setup(bot):
    await bot.add_cog(DomainLookup(bot))