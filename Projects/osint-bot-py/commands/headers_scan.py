import aiohttp
import discord
from discord.ext import commands

class HeaderScan(commands.Cog):
    def __init__(self, bot):
        self.bot = bot

    @commands.command()
    async def headers(self, ctx, url: str):
        """Mostra gli header HTTP di un sito"""

        # Aggiunge https:// se manca
        if not url.startswith("http://") and not url.startswith("https://"):
            url = "https://" + url

        await ctx.send(f"🔍 Scansione header per: {url}")

        try:
            async with aiohttp.ClientSession() as session:
                async with session.get(url, allow_redirects=True) as response:
                    headers = response.headers
                    status = response.status
                    final_url = str(response.url)

        except Exception as e:
            await ctx.send(f"❌ Errore durante la richiesta: `{e}`")
            return

        embed = discord.Embed(
            title="🧩 HTTP Header Scan",
            description=f"URL finale: `{final_url}`\nStatus: **{status}**",
            color=discord.Color.blue()
        )

        # Mostra i primi 15 header per evitare spam
        count = 0
        for key, value in headers.items():
            if count >= 15:
                embed.add_field(
                    name="📌 Nota",
                    value="Mostrati solo i primi 15 header.",
                    inline=False
                )
                break

            embed.add_field(
                name=f"🔹 {key}",
                value=f"`{value}`",
                inline=False
            )
            count += 1

        embed.set_footer(text="HTTP Header Scanner")

        await ctx.send(embed=embed)

async def setup(bot):
    await bot.add_cog(HeaderScan(bot))